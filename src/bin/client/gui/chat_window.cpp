#include <QPushButton>

#include "client/gui/chat_window.hpp"

ChatWindow::ChatWindow(QWidget *parent) : QWidget(parent)
{
    this->setFixedSize(100, 50);
    // Create and position the button
    button = new QPushButton("Hello World", this);
    button->setGeometry(10, 10, 80, 30);
}