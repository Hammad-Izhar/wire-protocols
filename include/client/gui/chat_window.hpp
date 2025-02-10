#pragma once
#include <QWidget>
// #include <QPushButton>

class QPushButton;
class ChatWindow : public QWidget
{
    Q_OBJECT
public:
    explicit ChatWindow(QWidget *parent = nullptr);

private:
    QPushButton *button;
};