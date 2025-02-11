#include <QPushButton>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QTimer>

#include "client/gui/registration_tab.hpp"

RegistrationTab::RegistrationTab(QWidget *parent) : QWidget(parent)
{
    inputGroup = new QGroupBox("Login", this);

    QLineEdit *username = new QLineEdit(inputGroup);
    username->setObjectName("usernameInput");
    username->setPlaceholderText("Username");
    QRegularExpressionValidator *username_validator = new QRegularExpressionValidator(QRegularExpression("^\\S+$"), this);
    username->setValidator(username_validator);
    connect(username, &QLineEdit::textChanged, this, RegistrationTab::validate_text(username, username_validator));
    connect(username, &QLineEdit::returnPressed, this, &RegistrationTab::on_submit);

    QLineEdit *displayName = new QLineEdit(inputGroup);
    displayName->setObjectName("displayNameInput");
    displayName->setPlaceholderText("Display Name");
    QRegularExpressionValidator *displayName_validator = new QRegularExpressionValidator(QRegularExpression("^\\S+$"), this);
    displayName->setValidator(displayName_validator);
    connect(displayName, &QLineEdit::textChanged, this, RegistrationTab::validate_text(displayName, displayName_validator));
    connect(displayName, &QLineEdit::returnPressed, this, &RegistrationTab::on_submit);

    QLineEdit *password = new QLineEdit(inputGroup);
    password->setObjectName("passwordInput");
    password->setPlaceholderText("Password");
    password->setEchoMode(QLineEdit::Password);
    QRegularExpressionValidator *password_validator = new QRegularExpressionValidator(QRegularExpression("^\\S+$"), this);
    password->setValidator(password_validator);
    connect(password, &QLineEdit::textChanged, this, RegistrationTab::validate_text(password, password_validator));
    connect(password, &QLineEdit::returnPressed, this, &RegistrationTab::on_submit);

    QPushButton *loginButton = new QPushButton("Register", this);
    loginButton->setObjectName("registrationButton");
    connect(loginButton, &QPushButton::clicked, this, &RegistrationTab::on_submit);

    spinner = new Spinner(this);
    spinner->hide();

    QFormLayout *formLayout = new QFormLayout(inputGroup);
    formLayout->addRow("Username:", username);
    formLayout->addRow("Display Name:", displayName);
    formLayout->addRow("Password:", password);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(inputGroup);
    mainLayout->addWidget(loginButton);
    mainLayout->addWidget(spinner);
    mainLayout->setAlignment(Qt::AlignCenter);

    setWindowTitle("Login");
    setFixedSize(450, sizeHint().height());
}

void RegistrationTab::on_submit()
{
    QLineEdit *usernameInput = qobject_cast<QLineEdit *>(this->inputGroup->findChild<QLineEdit *>("usernameInput"));
    QLineEdit *displayNameInput = qobject_cast<QLineEdit *>(this->inputGroup->findChild<QLineEdit *>("displayNameInput"));
    QLineEdit *passwordInput = qobject_cast<QLineEdit *>(this->inputGroup->findChild<QLineEdit *>("passwordInput"));

    QString username = usernameInput->text();
    QString displayName = displayNameInput->text();
    QString password = passwordInput->text();

    bool is_valid_username = validate_text(usernameInput, usernameInput->validator())(username);
    bool is_valid_displayName = validate_text(displayNameInput, displayNameInput->validator())(displayName);
    bool is_valid_password = validate_text(passwordInput, passwordInput->validator())(password);
    if (!is_valid_username || !is_valid_displayName || !is_valid_password)
    {
        return;
    }

    qDebug() << username.toStdString().c_str() << ":" << displayName.toStdString().c_str() << ":" << password.toStdString().c_str();

    set_loading(true);
    QTimer::singleShot(5000, this, [this]()
                       { set_loading(false); });
}

void RegistrationTab::set_loading(bool isLoading)
{
    if (isLoading)
    {
        inputGroup->hide();
        this->findChild<QPushButton *>("registrationButton")->hide();
        spinner->show();
    }
    else
    {
        inputGroup->show();
        this->findChild<QPushButton *>("registrationButton")->show();
        spinner->hide();
    }
}

std::function<bool(const QString &)> RegistrationTab::validate_text(QLineEdit *widget, const QValidator *validator)
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