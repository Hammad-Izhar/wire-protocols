#pragma once
#include <QHBoxLayout>
#include <QStackedLayout>
#include <QWidget>

#include "client/gui/chat_area.hpp"
#include "client/gui/chat_sidebar.hpp"

class ChatWindow : public QWidget {
    Q_OBJECT

   public:
    explicit ChatWindow(QWidget* parent = nullptr);

   private:
    ChatSidebar* sidebar;
    ChatArea* chatArea;
    QStackedLayout* chatStack;
};
