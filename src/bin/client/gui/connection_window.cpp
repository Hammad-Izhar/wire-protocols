#include <QPushButton>
#include <QFormLayout>
#include <QIntValidator>
#include <QRegularExpressionValidator>
#include <QTimer>
#include <QMessageBox>
#include <cassert>

#include "client/gui/connection_window.hpp"
#include "client/gui/components/validated_text_input.hpp"
#include "client/model/session.hpp"

ConnectionWindow::ConnectionWindow(QWidget *parent) : QWidget(parent)
{
    // Group box to visually group connection fields
    inputGroup = new QGroupBox("", this);

    // Hostname field
    QRegularExpressionValidator *host_validator = new QRegularExpressionValidator(QRegularExpression("^\\S+$"), this);
    ValidatedTextInput *host = new ValidatedTextInput(
        inputGroup,
        "hostInput",
        "Hostname",
        host_validator,
        QLineEdit::EchoMode::Normal,
        validate_text(host_validator),
        std::bind(&ConnectionWindow::on_submit, this));

    // Port field
    QIntValidator *port_validator = new QIntValidator(0, 65535, this);
    ValidatedTextInput *port = new ValidatedTextInput(
        inputGroup,
        "portInput",
        "Port",
        port_validator,
        QLineEdit::EchoMode::Normal,
        validate_text(port_validator),
        std::bind(&ConnectionWindow::on_submit, this));

    // Connect button
    QPushButton *connectButton = new QPushButton("Connect", this);
    connectButton->setObjectName("connectButton");
    connect(connectButton, &QPushButton::clicked, this, &ConnectionWindow::on_submit);

    spinner = new Spinner(this);
    spinner->hide();

    QGridLayout *gridLayout = new QGridLayout(inputGroup);
    gridLayout->addWidget(new QLabel("Hostname:"), 0, 0, Qt::AlignRight | Qt::AlignVCenter);
    gridLayout->addWidget(host, 0, 1, Qt::AlignLeft | Qt::AlignVCenter);
    gridLayout->addWidget(new QLabel("Port:"), 1, 0, Qt::AlignRight | Qt::AlignVCenter);
    gridLayout->addWidget(port, 1, 1, Qt::AlignLeft | Qt::AlignVCenter);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(inputGroup);
    mainLayout->addWidget(connectButton);
    mainLayout->addWidget(spinner);
    mainLayout->setAlignment(Qt::AlignCenter);

    setWindowTitle("Connect to Server");
    setFixedSize(450, sizeHint().height());
}

void ConnectionWindow::set_loading(bool isLoading)
{
    if (isLoading)
    {
        inputGroup->hide();
        this->findChild<QPushButton *>("connectButton")->hide();
        spinner->show();
    }
    else
    {
        inputGroup->show();
        this->findChild<QPushButton *>("connectButton")->show();
        spinner->hide();
    }
}

void ConnectionWindow::on_submit()
{
    QLineEdit *hostInput = qobject_cast<QLineEdit *>(this->inputGroup->findChild<QLineEdit *>("hostInput"));
    QLineEdit *portInput = qobject_cast<QLineEdit *>(this->inputGroup->findChild<QLineEdit *>("portInput"));

    QString hostname = hostInput->text();
    QString port = portInput->text();

    set_loading(true);
    bool is_valid_hostname = validate_text(hostInput->validator())(hostname, hostInput);
    bool is_valid_port = validate_text(portInput->validator())(port, portInput);

    if (!is_valid_hostname || !is_valid_port)
    {
        set_loading(false);
        return;
    }

    qDebug() << hostname.toStdString().c_str() << ":" << port.toStdString().c_str();

    Session &session = Session::getInstance();

    {
        QMutexLocker lock(&session.session_mutex);
        session.connection_address = std::make_pair(hostname.toStdString(), port.toUInt());
        session.worker_cond_var.wakeAll();
    }

    // Instead of blocking, check asynchronously for connection updates
    QTimer::singleShot(100, this, &ConnectionWindow::check_connection_status);
}

void ConnectionWindow::check_connection_status()
{
    Session &session = Session::getInstance();

    if (session.session_mutex.tryLock()) // Try to lock without blocking
    {

        if (session.socket_fd.has_value())
        {
            qDebug() << "Connected successfully! on socket fd: " << session.socket_fd.value();
            set_loading(false);
            session.session_mutex.unlock();
            return;
        }

        if (session.error.has_value())
        {
            qDebug() << "Connection error: " << QString::fromStdString(session.error.value());
            session.error.reset();

            QLineEdit *hostInput = qobject_cast<QLineEdit *>(this->inputGroup->findChild<QLineEdit *>("hostInput"));
            QLineEdit *portInput = qobject_cast<QLineEdit *>(this->inputGroup->findChild<QLineEdit *>("portInput"));

            hostInput->setStyleSheet("border: 2px solid red; color: red;");
            portInput->setStyleSheet("border: 2px solid red; color: red;");
            hostInput->clear();
            portInput->clear();

            set_loading(false);
            session.session_mutex.unlock();
            return;
        }

        assert(false && "Neither socket_fd nor error was set by worker thread"); // Should not reach here
    }

    // If tryLock() fails, schedule another check
    QTimer::singleShot(100, this, &ConnectionWindow::check_connection_status);
}

std::function<bool(const QString &, QWidget *)> ConnectionWindow::validate_text(const QValidator *validator)
{
    return [=](const QString &text, QWidget *widget)
    {
        QString text_copy = QString(text);
        int pos;
        if (validator->validate(text_copy, pos) == QValidator::Intermediate)
        {
            widget->setStyleSheet("border: 2px solid red; color: red;");
            return false;
        }

        widget->setStyleSheet(""); // Reset to default
        return true;
    };
}