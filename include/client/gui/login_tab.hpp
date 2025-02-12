#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QGroupBox>
#include <QValidator>
#include <QString>
#include <QPushButton>
#include <QTabWidget>

#include "client/gui/components/spinner.hpp"

class LoginTab : public QWidget
{
    Q_OBJECT
public:
    explicit LoginTab(QTabWidget *tabWidget, QWidget *parent = nullptr);

private:
    QGroupBox *inputGroup;
    Spinner *spinner;
    QTabWidget *tabWidget;

    void
    on_submit();

private slots:
    void onLoginSuccess();
    void onLoginFailure(const QString &errorMessage);

    void set_loading(bool isLoading);

    static std::function<bool(const QString &, QWidget *)> validate_text(const QValidator *validator);
};