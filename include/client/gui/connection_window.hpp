#pragma once
#include <QGroupBox>
#include <QLineEdit>
#include <QString>
#include <QValidator>
#include <QWidget>

#include "client/gui/components/spinner.hpp"

class ConnectionWindow : public QWidget {
    Q_OBJECT
   public:
    explicit ConnectionWindow(QWidget* parent = nullptr);

   private:
    QGroupBox* inputGroup;
    Spinner* spinner;

    void on_submit();

    void set_loading(bool isLoading);

    std::function<bool(const QString&, QWidget*)> validate_text(const QValidator* validator);
};