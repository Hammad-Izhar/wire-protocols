#include "client/gui/chat_area.hpp"
#include <qglobal.h>
#include "client/gui/components/message_widget.hpp"
#include "client/model/session.hpp"

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

    sendButton = new QPushButton("Send", this);
    sendButton->setStyleSheet("padding: 5px 10px;");
    sendButton->setEnabled(false);  // Initially disabled
    connect(sendButton, &QPushButton::clicked, this, [this]() {
        // Handle sending message (for now, just clear input)
        messageInput->clear();
    });

    inputLayout->addWidget(messageInput);
    inputLayout->addWidget(sendButton);

    // Add all components to main layout
    layout->addWidget(chatTitle);
    layout->addWidget(messageScrollArea);
    layout->addLayout(inputLayout);  // Add input field + send button

    setLayout(layout);

    Session& session = Session::get_instance();
    connect(&session, &Session::updateActiveChannel, this, &ChatArea::onActiveChannelChanged);
}

void ChatArea::validateMessage() {
    Session& session = Session::get_instance();
    sendButton->setEnabled(session.get_active_channel().has_value() &&
                           !messageInput->text().isEmpty());
}

void ChatArea::onActiveChannelChanged() {
    Session& session = Session::get_instance();
    qDebug() << "Updating channel";
    if (session.get_active_channel().has_value()) {
        messageInput->setEnabled(true);
        chatTitle->setText(
            QString::fromStdString(session.get_active_channel().value()->get_name()));
    } else {
        chatTitle->setText("");
    }
}
