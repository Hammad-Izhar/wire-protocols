#pragma once
#include <QHBoxLayout>
#include <QStackedLayout>
#include <QWidget>

#include "client/gui/chat_area.hpp"
#include "client/gui/chat_sidebar.hpp"

/**
 * @brief ChatWindow class
 * 
 * This class represents the main chat window of the application.
 * It contains a sidebar and a chat area.
 */
class ChatWindow : public QWidget {
    Q_OBJECT

   public:
    /**
     * @brief Construct a new ChatWindow object
     * 
     * @param parent 
     */
    explicit ChatWindow(QWidget* parent = nullptr);

    /**
     * @brief Set the sidebar object
     * 
     * @param sidebar 
     */
    void reset();

   private:
    ChatSidebar* sidebar;
    ChatArea* chatArea;

   private slots:
    void onDisconnect();
    void onDeleteAccount();
};
