#include <QScrollBar>
#include <QTimer>

#include <qdebug.h>
#include "client/gui/chat_area.hpp"
#include "client/gui/components/message_widget.hpp"
#include "client/model/session.hpp"
#include "client/model/tcp_client.hpp"
#include "models/message.hpp"

ChatArea::ChatArea(QWidget* parent) : QWidget(parent) {
    QVBoxLayout* layout = new QVBoxLayout(this);

    // Chat title at the top
    chatTitle = new QLabel("", this);
    chatTitle->setStyleSheet("font-size: 16px; font-weight: bold; padding: 5px;");

    // Scrollable message area
    messageScrollArea = new QScrollArea(this);
    messageContainer = new QWidget();
    messageLayout = new QVBoxLayout(messageContainer);

    messageContainer->setLayout(messageLayout);
    messageScrollArea->setWidget(messageContainer);
    messageScrollArea->setWidgetResizable(true);

    // Message input area (input field + send button)
    QHBoxLayout* inputLayout = new QHBoxLayout();

    messageInput = new QLineEdit(this);
    messageInput->setPlaceholderText("Type a message...");
    messageInput->setStyleSheet("padding: 5px;");
    messageInput->setMaxLength(255);  // Enforces max length
    messageInput->setEnabled(false);  // Initially disabled
    connect(messageInput, &QLineEdit::textChanged, this, &ChatArea::validateMessage);
    connect(messageInput, &QLineEdit::returnPressed, this, &ChatArea::sendMessage);

    sendButton = new QPushButton("Send", this);
    sendButton->setStyleSheet("padding: 5px 10px;");
    sendButton->setEnabled(false);  // Initially disabled
    connect(sendButton, &QPushButton::clicked, this, &ChatArea::sendMessage);

    inputLayout->addWidget(messageInput);
    inputLayout->addWidget(sendButton);

    // Add all components to main layout
    layout->addWidget(chatTitle);
    layout->addWidget(messageScrollArea);
    layout->addLayout(inputLayout);  // Add input field + send button

    setLayout(layout);

    Session& session = Session::get_instance();
    connect(&session, &Session::updateActiveChannel, this, &ChatArea::onActiveChannelChanged);
    connect(session.tcp_client, &TcpClient::sendMessageSuccess, this,
            &ChatArea::onSendMessageSuccess);
    connect(session.tcp_client, &TcpClient::sendMessageFailure, this,
            &ChatArea::onSendMessageFailure);
    connect(session.tcp_client, &TcpClient::deleteMessageSuccess, this,
            &ChatArea::onDeleteMessageSuccess);
    connect(session.tcp_client, &TcpClient::deleteMessageFailure, this,
            &ChatArea::onDeleteMessageFailure);
}

void ChatArea::validateMessage() {
    Session& session = Session::get_instance();
    sendButton->setEnabled(session.get_active_channel().has_value() &&
                           !messageInput->text().isEmpty());
}

void ChatArea::sendMessage() {
    Session& session = Session::get_instance();
    if (session.get_active_channel().has_value() && !messageInput->text().isEmpty()) {
        session.tcp_client->send_text_message(session.get_active_channel().value()->get_uid(),
                                              session.authenticated_user.value()->get_uid(),
                                              messageInput->text().toStdString());
        messageInput->clear();
    }
}

void ChatArea::onActiveChannelChanged() {
    Session& session = Session::get_instance();
    if (session.get_active_channel().has_value()) {
        messageInput->setEnabled(true);
        chatTitle->setText(
            QString::fromStdString(session.get_active_channel().value()->get_name()));

        // Clear existing messages
        QLayoutItem* child;
        while ((child = messageLayout->takeAt(0)) != nullptr) {
            delete child->widget();
            delete child;
        }

        for (auto& message : session.get_active_channel_messages()) {
            addMessageToLayout(message);
        }

        // Auto-scroll to the bottom
        QTimer::singleShot(50, [this]() {
            messageScrollArea->verticalScrollBar()->setValue(
                messageScrollArea->verticalScrollBar()->maximum());
        });
    } else {
        chatTitle->setText("");
    }
}

void ChatArea::addMessageToLayout(Message::SharedPtr message) {
    Session& session = Session::get_instance();
    MessageWidget* messageWidget = new MessageWidget(message, messageContainer);
    messageLayout->addWidget(messageWidget);
    messageLayout->update();
    messageContainer->adjustSize();
}

void ChatArea::onSendMessageSuccess(Message::SharedPtr message) {
    Session& session = Session::get_instance();
    if (session.get_active_channel().has_value() &&
        session.get_active_channel().value()->get_uid() == message->get_channel_id()) {
        MessageWidget* messageWidget = new MessageWidget(message, messageContainer);
        messageLayout->addWidget(messageWidget);
    }

    messageLayout->update();
    messageContainer->adjustSize();
}

void ChatArea::onSendMessageFailure(const QString& error) {
    qDebug() << "Failed to send message: " << error;
}

void ChatArea::onDeleteMessageSuccess(Message::SharedPtr message) {
    Session& session = Session::get_instance();
    if (session.get_active_channel().has_value() &&
        session.get_active_channel().value()->get_uid() == message->get_channel_id()) {
        // Find and remove the corresponding MessageWidget
        for (int i = 0; i < messageLayout->count(); ++i) {
            MessageWidget* widget =
                dynamic_cast<MessageWidget*>(messageLayout->itemAt(i)->widget());
            if (widget && widget->getMessage()->get_snowflake() == message->get_snowflake()) {
                QWidget* itemWidget = messageLayout->itemAt(i)->widget();
                messageLayout->removeWidget(itemWidget);
                delete itemWidget;
                break;
            }
        }
    }

    messageLayout->update();
    messageContainer->adjustSize();
}

void ChatArea::onDeleteMessageFailure(const QString& error) {
    qDebug() << "Failed to delete message: " << error;
}

void ChatArea::reset() {
    messageInput->setEnabled(false);
    messageInput->clear();
    sendButton->setEnabled(false);
    chatTitle->setText("");
}