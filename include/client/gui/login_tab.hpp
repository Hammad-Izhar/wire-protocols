#pragma once
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QString>
#include <QTabWidget>
#include <QValidator>
#include <QWidget>

#include "client/gui/components/spinner.hpp"

/**
 * @brief The LoginTab class represents a tab in the GUI for user login.
 * 
 * This class provides the user interface elements and functionality for 
 * logging into the application. It includes input fields for user credentials,
 * a submit button, and a spinner to indicate loading state. It also handles 
 * login success and failure events.
 */
class LoginTab : public QWidget {
    Q_OBJECT
   public:
    explicit LoginTab(QTabWidget* tabWidget, QWidget* parent = nullptr);

    void reset();

   private:
    QGroupBox* inputGroup;
    Spinner* spinner;
    QTabWidget* tabWidget;

    void on_submit();

   private slots:
    void onLoginSuccess();
    void onLoginFailure(const QString& errorMessage);

    void set_loading(bool isLoading);

    static std::function<bool(const QString&, QWidget*)> validate_text(const QValidator* validator);
};