#include <QTabWidget>
#include <QVBoxLayout>

#include "client/gui/authentication_window.hpp"

AuthenticationWindow::AuthenticationWindow(QWidget *parent) : QWidget(parent)
{
    QTabWidget *tabs = new QTabWidget(this);

    loginTab = new LoginTab(this);
    registrationTab = new RegistrationTab(tabs, this);

    tabs->addTab(loginTab, "Login");
    tabs->addTab(registrationTab, "Register");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(tabs);
    mainLayout->setAlignment(Qt::AlignCenter);

    setFixedSize(450, sizeHint().height());
    setWindowTitle("Login to [...]");

    connect(tabs, &QTabWidget::currentChanged, this, [this](int index)
            {
        if (index == 0)
        {
            setWindowTitle("Login to [...]");
        }
        else
        {
            setWindowTitle("Register to [...]");
        } });
}
