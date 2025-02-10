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
    QLineEdit *host;
    QLineEdit *port;
    QMovie *spinnerMovie;
    QLabel *spinnerLabel;

    std::function<void(const QString &)> validate_text(QLineEdit *widget, const QValidator *validator);
    void on_submit();
};