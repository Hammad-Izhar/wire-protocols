#include <QLineEdit>
#include <QIntValidator>
#include <QPushButton>
#include <iostream>
#include <QTimer>
#include <QHBoxLayout>
#include <QFormLayout>

#include "client/gui/connection_window.hpp"

ConnectionWindow::ConnectionWindow(QWidget *parent) : QWidget(parent)
{
    inputGroup = new QGroupBox("Server Address", this);

    host = new QLineEdit(inputGroup);
    QRegularExpressionValidator *host_validator = new QRegularExpressionValidator(QRegularExpression("^\\S+$"), this);
    host->setValidator(host_validator);
    host->setGeometry(10, 10, 150, 30);
    host->setPlaceholderText("Hostname (eg. 127.0.0.1)");
    connect(host, &QLineEdit::textChanged, this, ConnectionWindow::validate_text(host, host_validator));
    connect(host, &QLineEdit::returnPressed, this, &ConnectionWindow::on_submit);

    port = new QLineEdit(inputGroup);
    QIntValidator *port_validator = new QIntValidator(0, 65535, this);
    port->setGeometry(170, 10, 120, 30);
    port->setValidator(port_validator);
    port->setPlaceholderText("Port (eg. 25565)");
    connect(port, &QLineEdit::textChanged, this, ConnectionWindow::validate_text(port, port_validator));
    connect(port, &QLineEdit::returnPressed, this, &ConnectionWindow::on_submit);

    QPushButton *submit = new QPushButton("Connect", inputGroup);
    submit->setGeometry(300, 10, 80, 30);
    connect(submit, &QPushButton::clicked, this, &ConnectionWindow::on_submit);

    QFormLayout *formLayout = new QFormLayout(inputGroup);
    formLayout->addRow("Hostname:", host);
    formLayout->addRow("Port:", host);
    formLayout->addRow(submit);

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(inputGroup);
    setLayout(mainLayout);

    spinnerMovie = new QMovie(":/assets/animations/loading.gif");
    spinnerLabel = new QLabel(this);
    spinnerLabel->setMovie(spinnerMovie);
    spinnerLabel->hide(); // Hidden by default

    setWindowTitle("Connect to Server");
    setFixedSize(400, 300);
    setStyleSheet("QLineEdit { padding: 5px; }");
    setStyleSheet("QPushButton { padding: 5px; }");
    setStyleSheet("QLabel { padding: 5px; }");
    setStyleSheet("QWidget { background-color: #2e3440; color: #d8dee9; }");
    setStyleSheet("QLineEdit { background-color: #3b4252; color: #d8dee9; border: 1px solid #4c566a; }");
    setStyleSheet("QPushButton { background-color: #4c566a; color: #d8dee9; border: none; }");
    setStyleSheet("QPushButton:hover { background-color: #434c5e; }");
    setStyleSheet("QPushButton:pressed { background-color: #3b4252; }");
}

void ConnectionWindow::on_submit()
{
    QString hostname = this->host->text();
    QString portNumber = this->port->text();

    validate_text(this->host, this->host->validator())(hostname);
    validate_text(this->port, this->port->validator())(portNumber);

    std::cout << hostname.toStdString() << ":" << portNumber.toStdString() << std::endl;

    inputGroup->hide();
    spinnerLabel->show();
    spinnerMovie->start(); // Start the loading animation
    // Simulate connection delay (e.g., 5 seconds), then restore UI
    QTimer::singleShot(5000, this, [=]()
                       {
        spinnerLabel->hide();
        spinnerMovie->stop(); 
        inputGroup->show(); });
}

std::function<void(const QString &)> ConnectionWindow::validate_text(QLineEdit *widget, const QValidator *validator)
{
    return [=](const QString &text)
    {
        QString text_copy = QString(text);
        int pos;
        if (validator->validate(text_copy, pos) == QValidator::Intermediate)
        {
            widget->setStyleSheet("border: 2px solid red; color: red;");
        }
        else
        {
            widget->setStyleSheet(""); // Reset to default
        }
    };
}