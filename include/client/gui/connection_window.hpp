#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QProgressBar>
#include <QMovie>
#include <QLabel>
#include <QGroupBox>

class QPushButton;
class ConnectionWindow : public QWidget
{
    Q_OBJECT
public:
    explicit ConnectionWindow(QWidget *parent = nullptr);

private:
    QGroupBox *inputGroup;
    QMovie *spinnerMovie;
    QLabel *spinnerLabel;

    void on_submit();

    void set_loading(bool isLoading);

    std::function<bool(const QString &)> validate_text(QLineEdit *widget, const QValidator *validator);
};