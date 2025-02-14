#pragma once
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>

#include "models/message.hpp"

class ChatArea : public QWidget {
    Q_OBJECT

   public:
    explicit ChatArea(QWidget* parent = nullptr);

   private:
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
