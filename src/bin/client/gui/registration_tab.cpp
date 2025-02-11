#include <QPushButton>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QTimer>

#include "client/gui/registration_tab.hpp"
#include "client/gui/components/validated_text_input.hpp"

RegistrationTab::RegistrationTab(QWidget *parent) : QWidget(parent)
{
    inputGroup = new QGroupBox("", this);

    QRegularExpressionValidator *username_validator = new QRegularExpressionValidator(QRegularExpression("^\\S+$"), this);
    ValidatedTextInput *username = new ValidatedTextInput(
        inputGroup,
        "usernameInput",
        "Username",
        username_validator,
        QLineEdit::EchoMode::Normal,
        validate_text(username_validator),
        std::bind(&RegistrationTab::on_submit, this));

    QRegularExpressionValidator *displayName_validator = new QRegularExpressionValidator(QRegularExpression("^\\S+$"), this);
    ValidatedTextInput *displayName = new ValidatedTextInput(
        inputGroup,
        "displayNameInput",
        "Display Name",
        displayName_validator,
        QLineEdit::EchoMode::Normal,
        validate_text(displayName_validator),
        std::bind(&RegistrationTab::on_submit, this));

    QRegularExpressionValidator *password_validator = new QRegularExpressionValidator(QRegularExpression("^\\S+$"), this);
    ValidatedTextInput *password = new ValidatedTextInput(
        inputGroup,
        "passwordInput",
        "Password",
        password_validator,
        QLineEdit::EchoMode::Password,
        validate_text(password_validator),
        std::bind(&RegistrationTab::on_submit, this));

    QPushButton *loginButton = new QPushButton("Register", this);
    loginButton->setObjectName("registrationButton");
    connect(loginButton, &QPushButton::clicked, this, &RegistrationTab::on_submit);

    spinner = new Spinner(this);
    spinner->hide();

    QGridLayout *gridLayout = new QGridLayout(inputGroup);
    gridLayout->addWidget(new QLabel("Username:"), 0, 0, Qt::AlignRight | Qt::AlignVCenter);
    gridLayout->addWidget(username, 0, 1, Qt::AlignLeft | Qt::AlignVCenter);
    gridLayout->addWidget(new QLabel("Display Name:"), 1, 0, Qt::AlignRight | Qt::AlignVCenter);
    gridLayout->addWidget(displayName, 1, 1, Qt::AlignLeft | Qt::AlignVCenter);
    gridLayout->addWidget(new QLabel("Password:"), 2, 0, Qt::AlignRight | Qt::AlignVCenter);
    gridLayout->addWidget(password, 2, 1, Qt::AlignLeft | Qt::AlignVCenter);

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

    bool is_valid_username = validate_text(usernameInput->validator())(username, usernameInput);
    bool is_valid_displayName = validate_text(displayNameInput->validator())(displayName, displayNameInput);
    bool is_valid_password = validate_text(passwordInput->validator())(password, passwordInput);
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

std::function<bool(const QString &, QWidget *)> RegistrationTab::validate_text(const QValidator *validator)
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