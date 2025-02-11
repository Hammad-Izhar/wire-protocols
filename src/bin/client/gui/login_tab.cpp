#include <QPushButton>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QTimer>

#include "client/gui/login_tab.hpp"

LoginTab::LoginTab(QWidget *parent) : QWidget(parent)
{
    inputGroup = new QGroupBox("Login", this);

    QLineEdit *username = new QLineEdit(inputGroup);
    username->setObjectName("usernameInput");
    username->setPlaceholderText("Username");
    QRegularExpressionValidator *username_validator = new QRegularExpressionValidator(QRegularExpression("^\\S+$"), this);
    username->setValidator(username_validator);
    connect(username, &QLineEdit::textChanged, this, LoginTab::validate_text(username, username_validator));
    connect(username, &QLineEdit::returnPressed, this, &LoginTab::on_submit);

    QLineEdit *password = new QLineEdit(inputGroup);
    password->setObjectName("passwordInput");
    password->setPlaceholderText("Password");
    password->setEchoMode(QLineEdit::Password);
    QRegularExpressionValidator *password_validator = new QRegularExpressionValidator(QRegularExpression("^\\S+$"), this);
    password->setValidator(password_validator);
    connect(password, &QLineEdit::textChanged, this, LoginTab::validate_text(password, password_validator));
    connect(password, &QLineEdit::returnPressed, this, &LoginTab::on_submit);

    QPushButton *loginButton = new QPushButton("Login", this);
    loginButton->setObjectName("loginButton");
    connect(loginButton, &QPushButton::clicked, this, &LoginTab::on_submit);

    spinner = new Spinner(this);
    spinner->hide();

    QFormLayout *formLayout = new QFormLayout(inputGroup);
    formLayout->addRow("Username:", username);
    formLayout->addRow("Password:", password);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(inputGroup);
    mainLayout->addWidget(loginButton);
    mainLayout->addWidget(spinner);
    mainLayout->setAlignment(Qt::AlignCenter);

    setWindowTitle("Login");
    setFixedSize(450, sizeHint().height());
}

void LoginTab::on_submit()
{
    QLineEdit *usernameInput = qobject_cast<QLineEdit *>(this->inputGroup->findChild<QLineEdit *>("usernameInput"));
    QLineEdit *passwordInput = qobject_cast<QLineEdit *>(this->inputGroup->findChild<QLineEdit *>("passwordInput"));

    QString username = usernameInput->text();
    QString password = passwordInput->text();

    bool is_valid_username = validate_text(usernameInput, usernameInput->validator())(username);
    bool is_valid_password = validate_text(passwordInput, passwordInput->validator())(password);
    if (!is_valid_username || !is_valid_password)
    {
        return;
    }

    qDebug() << username.toStdString().c_str() << ":" << password.toStdString().c_str();

    set_loading(true);
    QTimer::singleShot(5000, this, [this]()
                       { set_loading(false); });
}

void LoginTab::set_loading(bool isLoading)
{
    if (isLoading)
    {
        inputGroup->hide();
        this->findChild<QPushButton *>("loginButton")->hide();
        spinner->show();
    }
    else
    {
        inputGroup->show();
        this->findChild<QPushButton *>("loginButton")->show();
        spinner->hide();
    }
}

std::function<bool(const QString &)> LoginTab::validate_text(QLineEdit *widget, const QValidator *validator)
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