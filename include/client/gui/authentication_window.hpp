#pragma once
#include <QWidget>

#include "client/gui/login_tab.hpp"
#include "client/gui/registration_tab.hpp"

enum Tab { Login = 0, Register = 1 };

class AuthenticationWindow : public QWidget {
    Q_OBJECT
   public:
    explicit AuthenticationWindow(QWidget* parent = nullptr);

    void reset();

   private:
    LoginTab* loginTab;
    RegistrationTab* registrationTab;
};