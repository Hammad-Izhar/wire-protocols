#pragma once
#include <QWidget>

#include "client/gui/login_tab.hpp"
#include "client/gui/registration_tab.hpp"

enum Tab { Login = 0, Register = 1 };

/**
 * @brief AuthenticationWindow class
 * This class is responsible for the authentication window
 * It contains the login and registration tabs
 */
class AuthenticationWindow : public QWidget {
    Q_OBJECT
   public:

    /**
     * @brief Construct a new Authentication Window object
     * 
     * @param parent 
     */
    explicit AuthenticationWindow(QWidget* parent = nullptr);

    void reset();

   private:
    LoginTab* loginTab;
    RegistrationTab* registrationTab;
};