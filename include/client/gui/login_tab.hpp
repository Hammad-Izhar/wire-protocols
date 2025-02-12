#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QGroupBox>
#include <QValidator>
#include <QString>
#include <QPushButton>

#include "client/gui/components/spinner.hpp"

class LoginTab : public QWidget
{
    Q_OBJECT
public:
    explicit LoginTab(QWidget *parent = nullptr);

private:
    QGroupBox *inputGroup;
    Spinner *spinner;

    void on_submit();

    void set_loading(bool isLoading);

    static std::function<bool(const QString &, QWidget *)> validate_text(const QValidator *validator);
};