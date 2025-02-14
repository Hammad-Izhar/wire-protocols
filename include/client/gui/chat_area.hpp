#pragma once
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>

#include "models/message.hpp"

/**
 * @brief ChatArea class
 * This class is responsible for displaying the chat area of the application.
 * It contains the chat title, the messages, the message input and the send button.
 * It also handles the sending of messages and the display of messages.

 */
class ChatArea : public QWidget {
    Q_OBJECT

   public:
    /**
     * @brief Construct a new Chat Area object
     * 
     * @param parent 
     */
    explicit ChatArea(QWidget* parent = nullptr);

    /**
     * @brief Destroy the Chat Area object
     * 
     */
    void reset();

   private:
    /**
     * @brief Add a message to the layout
     * 
     * @param message 
     */
    void addMessageToLayout(Message::SharedPtr message);

   private slots:
    void validateMessage();
    void onActiveChannelChanged();
    void sendMessage();
    void onSendMessageSuccess(Message::SharedPtr message);
    void onSendMessageFailure(const QString& error_message);
    void onDeleteMessageSuccess(Message::SharedPtr message);
    void onDeleteMessageFailure(const QString& error_message);

   private:
    QLabel* chatTitle;
    QScrollArea* messageScrollArea;
    QWidget* messageContainer;
    QVBoxLayout* messageLayout;
    QLineEdit* messageInput;
    QPushButton* sendButton;
};
