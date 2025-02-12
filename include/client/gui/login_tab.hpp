#pragma once
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QString>
#include <QTabWidget>
#include <QValidator>
#include <QWidget>

#include "client/gui/components/spinner.hpp"

class LoginTab : public QWidget {
    Q_OBJECT
   public:
    explicit LoginTab(QTabWidget* tabWidget, QWidget* parent = nullptr);

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