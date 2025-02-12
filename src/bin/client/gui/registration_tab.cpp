#include <QFormLayout>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>

#include "client/gui/authentication_window.hpp"
#include "client/gui/components/validated_text_input.hpp"
#include "client/gui/registration_tab.hpp"
#include "client/model/session.hpp"

RegistrationTab::RegistrationTab(QTabWidget* tabWidget, QWidget* parent)
    : QWidget(parent), tabWidget(tabWidget) {
    inputGroup = new QGroupBox("", this);

    QRegularExpressionValidator* username_validator =
        new QRegularExpressionValidator(QRegularExpression("^\\S+$"), this);
    ValidatedTextInput* username = new ValidatedTextInput(
        inputGroup, "usernameInput", "Username", username_validator, QLineEdit::EchoMode::Normal,
        validate_text(username_validator), std::bind(&RegistrationTab::on_submit, this));

    QRegularExpressionValidator* displayName_validator =
        new QRegularExpressionValidator(QRegularExpression("^\\S+$"), this);
    ValidatedTextInput* displayName = new ValidatedTextInput(
        inputGroup, "displayNameInput", "Display Name", displayName_validator,
        QLineEdit::EchoMode::Normal, validate_text(displayName_validator),
        std::bind(&RegistrationTab::on_submit, this));

    QRegularExpressionValidator* password_validator =
        new QRegularExpressionValidator(QRegularExpression("^\\S+$"), this);
    ValidatedTextInput* password = new ValidatedTextInput(
        inputGroup, "passwordInput", "Password", password_validator, QLineEdit::EchoMode::Password,
        validate_text(password_validator), std::bind(&RegistrationTab::on_submit, this));

    QPushButton* loginButton = new QPushButton("Register", this);
    loginButton->setObjectName("registrationButton");
    connect(loginButton, &QPushButton::clicked, this, &RegistrationTab::on_submit);

    QPushButton* disconnectButton = new QPushButton("Disconnect", this);
    disconnectButton->setObjectName("disconnectButton");
    connect(disconnectButton, &QPushButton::clicked, this, [this]() {
        Session& session = Session::getInstance();
        session.tcp_client->disconnectFromServer();
    });

    spinner = new Spinner(this);
    spinner->hide();

    QGridLayout* gridLayout = new QGridLayout(inputGroup);
    gridLayout->addWidget(new QLabel("Username:"), 0, 0, Qt::AlignRight | Qt::AlignVCenter);
    gridLayout->addWidget(username, 0, 1, Qt::AlignLeft | Qt::AlignVCenter);
    gridLayout->addWidget(new QLabel("Display Name:"), 1, 0, Qt::AlignRight | Qt::AlignVCenter);
    gridLayout->addWidget(displayName, 1, 1, Qt::AlignLeft | Qt::AlignVCenter);
    gridLayout->addWidget(new QLabel("Password:"), 2, 0, Qt::AlignRight | Qt::AlignVCenter);
    gridLayout->addWidget(password, 2, 1, Qt::AlignLeft | Qt::AlignVCenter);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(inputGroup);
    mainLayout->addWidget(loginButton);
    mainLayout->addWidget(disconnectButton);
    mainLayout->addWidget(spinner);
    mainLayout->setAlignment(Qt::AlignCenter);

    setFixedSize(450, sizeHint().height());

    Session& session = Session::getInstance();
    connect(session.tcp_client, &TcpClient::registrationSuccess, this,
            &RegistrationTab::onRegistrationSuccess);
    connect(session.tcp_client, &TcpClient::registrationFailure, this,
            &RegistrationTab::onRegistrationFailure);
}

void RegistrationTab::on_submit() {
    QLineEdit* usernameInput =
        qobject_cast<QLineEdit*>(this->inputGroup->findChild<QLineEdit*>("usernameInput"));
    QLineEdit* displayNameInput =
        qobject_cast<QLineEdit*>(this->inputGroup->findChild<QLineEdit*>("displayNameInput"));
    QLineEdit* passwordInput =
        qobject_cast<QLineEdit*>(this->inputGroup->findChild<QLineEdit*>("passwordInput"));

    QString username = usernameInput->text();
    QString displayName = displayNameInput->text();
    QString password = passwordInput->text();

    bool is_valid_username = validate_text(usernameInput->validator())(username, usernameInput);
    bool is_valid_displayName =
        validate_text(displayNameInput->validator())(displayName, displayNameInput);
    bool is_valid_password = validate_text(passwordInput->validator())(password, passwordInput);
    if (!is_valid_username || !is_valid_displayName || !is_valid_password) {
        return;
    }

    set_loading(true);
    Session& session = Session::getInstance();
    session.tcp_client->register_user(username.toStdString(), displayName.toStdString(),
                                      password.toStdString());
}

void RegistrationTab::onRegistrationSuccess() {
    set_loading(false);
    tabWidget->setCurrentIndex(Tab::Login);
}

void RegistrationTab::onRegistrationFailure(const QString& errorMessage) {
    set_loading(false);
    // Handle registration failure (e.g., show an error message)
    qDebug() << "Registration failed:" << errorMessage;
}

void RegistrationTab::set_loading(bool isLoading) {
    if (isLoading) {
        inputGroup->hide();
        this->findChild<QPushButton*>("registrationButton")->hide();
        spinner->show();
    } else {
        inputGroup->show();
        this->findChild<QPushButton*>("registrationButton")->show();
        spinner->hide();
    }
}

std::function<bool(const QString&, QWidget*)> RegistrationTab::validate_text(
    const QValidator* validator) {
    return [=](const QString& text, QWidget* widget) {
        QString text_copy = QString(text);
        int pos;
        if (validator->validate(text_copy, pos) == QValidator::Intermediate) {
            widget->setStyleSheet("border: 2px solid red; color: red;");
            return false;
        }

        widget->setStyleSheet("");  // Reset to default
        return true;
    };
}