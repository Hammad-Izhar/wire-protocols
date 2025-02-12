#pragma once
#include <QWidget>
#include <QStackedLayout>
#include <QHBoxLayout>

#include "client/gui/chat_sidebar.hpp"
#include "client/gui/chat_area.hpp"

class ChatWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ChatWindow(QWidget *parent = nullptr);

private:
    ChatSidebar *sidebar;
    ChatArea *chatArea;
    QStackedLayout *chatStack;
};
