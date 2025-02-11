#include <QPushButton>
#include <QFormLayout>
#include <QIntValidator>
#include <QRegularExpressionValidator>
#include <QTimer>

#include "client/gui/connection_window.hpp"

ConnectionWindow::ConnectionWindow(QWidget *parent) : QWidget(parent)
{
    // Group box to visually group connection fields
    inputGroup = new QGroupBox("Server Address", this);

    // Hostname field
    QLineEdit *host = new QLineEdit(inputGroup);
    host->setObjectName("hostInput");
    host->setPlaceholderText("Hostname (eg. 127.0.0.1)");
    QRegularExpressionValidator *host_validator = new QRegularExpressionValidator(QRegularExpression("^\\S+$"), this);
    host->setValidator(host_validator);
    connect(host, &QLineEdit::textChanged, this, ConnectionWindow::validate_text(host, host_validator));
    connect(host, &QLineEdit::returnPressed, this, &ConnectionWindow::on_submit);

    // Port field
    QLineEdit *port = new QLineEdit(inputGroup);
    port->setObjectName("portInput");
    port->setPlaceholderText("Port (eg. 25565)");
    port->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QIntValidator *port_validator = new QIntValidator(0, 65535, this);
    port->setValidator(port_validator);
    connect(port, &QLineEdit::textChanged, this, ConnectionWindow::validate_text(port, port_validator));
    connect(port, &QLineEdit::returnPressed, this, &ConnectionWindow::on_submit);

    // Connect button
    QPushButton *connectButton = new QPushButton("Connect", this);
    connectButton->setObjectName("connectButton");
    connect(connectButton, &QPushButton::clicked, this, &ConnectionWindow::on_submit);

    spinner = new Spinner(this);
    spinner->hide();

    // Layouts
    QFormLayout *formLayout = new QFormLayout(inputGroup);
    formLayout->addRow("Hostname:", host);
    formLayout->addRow("Port:", port);

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

    bool is_valid_hostname = validate_text(hostInput, hostInput->validator())(hostname);
    bool is_valid_port = validate_text(portInput, portInput->validator())(port);
    if (!is_valid_hostname || !is_valid_port)
    {
        return;
    }

    qDebug() << hostname.toStdString().c_str() << ":" << port.toStdString().c_str();

    set_loading(true);
    QTimer::singleShot(5000, this, [this]()
                       { set_loading(false); });
}

std::function<bool(const QString &)> ConnectionWindow::validate_text(QLineEdit *widget, const QValidator *validator)
{
    return [=](const QString &text)
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