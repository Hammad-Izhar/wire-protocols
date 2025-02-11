#pragma once
#include <QWidget>

#include "client/gui/login_tab.hpp"
#include "client/gui/registration_tab.hpp"

class AuthenticationWindow : public QWidget
{
    Q_OBJECT
public:
    explicit AuthenticationWindow(QWidget *parent = nullptr);

private:
    LoginTab *loginTab;
    RegistrationTab *registrationTab;
};