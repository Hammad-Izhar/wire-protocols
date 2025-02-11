#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QGroupBox>
#include <QValidator>

#include "client/gui/components/spinner.hpp"

class ConnectionWindow : public QWidget
{
    Q_OBJECT
public:
    explicit ConnectionWindow(QWidget *parent = nullptr);

private:
    QGroupBox *inputGroup;
    Spinner *spinner;

    void on_submit();

    void set_loading(bool isLoading);

    std::function<bool(const QString &)> validate_text(QLineEdit *widget, const QValidator *validator);
};