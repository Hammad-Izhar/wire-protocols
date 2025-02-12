#include "client/gui/chat_window.hpp"

ChatWindow::ChatWindow(QWidget *parent) : QWidget(parent)
{
    QHBoxLayout *layout = new QHBoxLayout(this);

    sidebar = new ChatSidebar(this);
    chatArea = new ChatArea(this);

    layout->addWidget(sidebar);
    layout->addWidget(chatArea, 1);

    setLayout(layout);
}