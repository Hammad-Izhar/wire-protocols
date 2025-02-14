#pragma once
#include <QTabWidget>
#include <QWidget>

#include "client/gui/active_chats_tab.hpp"
#include "client/gui/search_tab.hpp"

enum ChatSidebarTab { ACTIVE_CHATS = 0, SEARCH = 1 };

/**
 * @brief ChatSidebar class
 * 
 * This class is a QWidget that contains a QTabWidget with two tabs: ActiveChatsTab and SearchTab.
 * It emits a signal setActiveChatTab when the user changes the active tab.
 * 
 * @see ActiveChatsTab
 */
class ChatSidebar : public QWidget {
    Q_OBJECT

   public:
    /**
     * @brief Construct a new ChatSidebar object
     * 
     * @param parent 
     */
    explicit ChatSidebar(QWidget* parent = nullptr);

    /**
     * @brief Destroy the ChatSidebar object
     * 
     */
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
