#include <QFormLayout>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>

#include "client/gui/components/validated_text_input.hpp"
#include "client/gui/login_tab.hpp"
#include "client/model/session.hpp"

LoginTab::LoginTab(QTabWidget* tabWidget, QWidget* parent) : QWidget(parent), tabWidget(tabWidget) {
    inputGroup = new QGroupBox("", this);

    QRegularExpressionValidator* username_validator =
        new QRegularExpressionValidator(QRegularExpression("^\\S+$"), this);
    ValidatedTextInput* username = new ValidatedTextInput(
        inputGroup, "usernameInput", "Username", username_validator, QLineEdit::EchoMode::Normal,
        validate_text(username_validator), std::bind(&LoginTab::on_submit, this));

    QRegularExpressionValidator* password_validator =
        new QRegularExpressionValidator(QRegularExpression("^\\S+$"), this);
    ValidatedTextInput* password = new ValidatedTextInput(
        inputGroup, "passwordInput", "Password", password_validator, QLineEdit::EchoMode::Password,
        validate_text(password_validator), std::bind(&LoginTab::on_submit, this));

    QPushButton* loginButton = new QPushButton("Login", this);
    loginButton->setObjectName("loginButton");
    connect(loginButton, &QPushButton::clicked, this, &LoginTab::on_submit);

    QPushButton* disconnectButton = new QPushButton("Disconnect", this);
    disconnectButton->setObjectName("disconnectButton");
    connect(disconnectButton, &QPushButton::clicked, this, [this]() {
        Session& session = Session::get_instance();
        session.tcp_client->disconnectFromServer();
        session.reset();
    });

    spinner = new Spinner(this);
    spinner->hide();

    QGridLayout* gridLayout = new QGridLayout(inputGroup);
    gridLayout->addWidget(new QLabel("Username:"), 0, 0, Qt::AlignRight | Qt::AlignVCenter);
    gridLayout->addWidget(username, 0, 1, Qt::AlignLeft | Qt::AlignVCenter);
    gridLayout->addWidget(new QLabel("Password:"), 1, 0, Qt::AlignRight | Qt::AlignVCenter);
    gridLayout->addWidget(password, 1, 1, Qt::AlignLeft | Qt::AlignVCenter);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(inputGroup);
    mainLayout->addWidget(loginButton);
    mainLayout->addWidget(disconnectButton);
    mainLayout->addWidget(spinner);
    mainLayout->setAlignment(Qt::AlignCenter);

    Session& session = Session::get_instance();
    connect(session.tcp_client, &TcpClient::loginSuccess, this, &LoginTab::onLoginSuccess);
    connect(session.tcp_client, &TcpClient::loginFailure, this, &LoginTab::onLoginFailure);

    setFixedSize(450, sizeHint().height());
}

void LoginTab::on_submit() {
    QLineEdit* usernameInput =
        qobject_cast<QLineEdit*>(this->inputGroup->findChild<QLineEdit*>("usernameInput"));
    QLineEdit* passwordInput =
        qobject_cast<QLineEdit*>(this->inputGroup->findChild<QLineEdit*>("passwordInput"));

    QString username = usernameInput->text();
    QString password = passwordInput->text();

    bool is_valid_username = validate_text(usernameInput->validator())(username, usernameInput);
    bool is_valid_password = validate_text(passwordInput->validator())(password, passwordInput);
    if (!is_valid_username || !is_valid_password) {
        return;
    }

    set_loading(true);
    Session& session = Session::get_instance();
    session.tcp_client->login_user(username.toStdString(), password.toStdString());
}

void LoginTab::onLoginSuccess() {
    set_loading(false);
    Session& session = Session::get_instance();
    session.main_window->animatePageTransition(Window::MAIN);
}

void LoginTab::onLoginFailure(const QString& errorMessage) {
    set_loading(false);
    qDebug() << "Login failed: " << errorMessage;
}

void LoginTab::set_loading(bool isLoading) {
    if (isLoading) {
        inputGroup->hide();
        this->findChild<QPushButton*>("loginButton")->hide();
        spinner->show();
    } else {
        inputGroup->show();
        this->findChild<QPushButton*>("loginButton")->show();
        spinner->hide();
    }
}

void LoginTab::reset() {
    QLineEdit* usernameInput =
        qobject_cast<QLineEdit*>(this->inputGroup->findChild<QLineEdit*>("usernameInput"));
    QLineEdit* passwordInput =
        qobject_cast<QLineEdit*>(this->inputGroup->findChild<QLineEdit*>("passwordInput"));

    usernameInput->setText("");
    passwordInput->setText("");

    set_loading(false);
}

std::function<bool(const QString&, QWidget*)> LoginTab::validate_text(const QValidator* validator) {
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