#pragma once
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QString>
#include <QTabWidget>
#include <QValidator>
#include <QWidget>

#include "client/gui/components/spinner.hpp"

class RegistrationTab : public QWidget {
    Q_OBJECT
   public:
    explicit RegistrationTab(QTabWidget* tabWidget, QWidget* parent = nullptr);

    void reset();

   private slots:
    void onRegistrationSuccess();
    void onRegistrationFailure(const QString& errorMessage);

   private:
    QGroupBox* inputGroup;
    QTabWidget* tabWidget;
    Spinner* spinner;

    void on_submit();

    void set_loading(bool isLoading);

    static std::function<bool(const QString&, QWidget*)> validate_text(const QValidator* validator);
};