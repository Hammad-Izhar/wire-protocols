#pragma once
#include <QWidget>
#include <QMovie>
#include <QLabel>

class Spinner : public QWidget
{
    Q_OBJECT
public:
    explicit Spinner(QWidget *parent = nullptr);

private:
    QMovie *spinnerMovie;
    QLabel *spinnerLabel;
};