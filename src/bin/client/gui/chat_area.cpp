#include "client/gui/chat_area.hpp"
#include "client/gui/components/message_widget.hpp"

ChatArea::ChatArea(QWidget* parent) : QWidget(parent) {
    QVBoxLayout* layout = new QVBoxLayout(this);

    // Chat title at the top
    chatTitle = new QLabel("Chat Name", this);
    chatTitle->setStyleSheet("font-size: 16px; font-weight: bold; padding: 5px;");

    // Scrollable message area
    messageScrollArea = new QScrollArea(this);
    messageContainer = new QWidget();
    messageLayout = new QVBoxLayout(messageContainer);

    // Example messages
    messageLayout->addWidget(new MessageWidget("Hello!", false));
    messageLayout->addWidget(new MessageWidget("Hi there!", true));
    messageLayout->addWidget(new MessageWidget("How are you?", false));
    messageLayout->addWidget(new MessageWidget("I'm good, thanks!", true));

    messageContainer->setLayout(messageLayout);
    messageScrollArea->setWidget(messageContainer);
    messageScrollArea->setWidgetResizable(true);

    // Message input area (input field + send button)
    QHBoxLayout* inputLayout = new QHBoxLayout();

    messageInput = new QLineEdit(this);
    messageInput->setPlaceholderText("Type a message...");
    messageInput->setStyleSheet("padding: 5px;");
    messageInput->setMaxLength(255);  // Enforces max length
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
}

// Validates input to ensure it's within 280 characters
void ChatArea::validateMessage() {
    sendButton->setEnabled(!messageInput->text().isEmpty());
}
