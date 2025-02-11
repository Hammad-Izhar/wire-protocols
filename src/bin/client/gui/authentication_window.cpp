#include <QTabWidget>
#include <QVBoxLayout>

#include "client/gui/authentication_window.hpp"

AuthenticationWindow::AuthenticationWindow(QWidget *parent) : QWidget(parent)
{
    loginTab = new LoginTab(this);
    registrationTab = new RegistrationTab(this);

    QTabWidget *tabs = new QTabWidget(this);
    tabs->addTab(loginTab, "Login");
    tabs->addTab(registrationTab, "Register");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(tabs);
    mainLayout->setAlignment(Qt::AlignCenter);

    setWindowTitle("Authentication");
    setFixedSize(450, sizeHint().height());
}
