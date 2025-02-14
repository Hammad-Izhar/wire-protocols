#pragma once
#include <QTabWidget>
#include <QWidget>

#include "client/gui/active_chats_tab.hpp"
#include "client/gui/search_tab.hpp"

enum ChatSidebarTab { ACTIVE_CHATS = 0, SEARCH = 1 };

class ChatSidebar : public QWidget {
    Q_OBJECT

   public:
    explicit ChatSidebar(QWidget* parent = nullptr);

    void reset();

   private:
    QTabWidget* tabWidget;
    ActiveChatsTab* activeChatsTab;
    SearchTab* searchTab;

   signals:
    void setActiveChatTab(ChatSidebarTab tab);

   private slots:
    void onSetActiveChatTab(int index);
};
