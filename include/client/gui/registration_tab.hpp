#pragma once
#include <QWidget>
#include <QWidget>
#include <QLineEdit>
#include <QGroupBox>
#include <QValidator>
#include <QString>
#include <QPushButton>
#include <QTabWidget>

#include "client/gui/components/spinner.hpp"

class RegistrationTab : public QWidget
{
    Q_OBJECT
public:
    explicit RegistrationTab(QTabWidget *tabWidget, QWidget *parent = nullptr);

private slots:
    void onRegistrationSuccess();
    void onRegistrationFailure(const QString &errorMessage);

private:
    QGroupBox *inputGroup;
    QTabWidget *tabWidget;
    Spinner *spinner;

    void on_submit();

    void set_loading(bool isLoading);

    static std::function<bool(const QString &, QWidget *)> validate_text(const QValidator *validator);
};