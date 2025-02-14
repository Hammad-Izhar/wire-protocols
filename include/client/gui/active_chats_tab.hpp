#pragma once
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QWidget>

#include "models/channel.hpp"

/**
 * @class ActiveChatsTab
 * @brief A custom QWidget that displays a list of active chats.
 *
 * This class extends QWidget to create a tab that displays a list of active chats.
 * It provides functionality to create new channels and select existing chats.
 *
 * @param parent The parent widget of this widget. Defaults to nullptr.
 */
class ActiveChatsTab : public QWidget {
    Q_OBJECT

   public:
   /**
    * @brief Constructs a new ActiveChatsTab object.
    */
    explicit ActiveChatsTab(QWidget* parent = nullptr);

    /**
     * @brief Adds a new chat to the list of active chats.
     *  
     * @param channel The channel to add to the list of active chats.
     */
    void reset();

   private:
    QListWidget* activeChats;

   private slots:
    void onCreateChannelSuccess(Channel::SharedPtr channel);
    void onCreateChannelFailure(const QString& errorMessage);
    void onChatSelected(QListWidgetItem* item);
};