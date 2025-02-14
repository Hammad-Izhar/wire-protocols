#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QStyleOption>
#include <QVBoxLayout>
#include <QWidget>
#include "client/model/session.hpp"

#include "client/gui/components/message_widget.hpp"

MessageWidget::MessageWidget(Message::SharedPtr message, QWidget* parent)
    : QWidget(parent), message(message) {
    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setSpacing(5);  // Reduce spacing for compact look

    QLabel* messageLabel = new QLabel(QString::fromStdString(message->get_text()), this);
    messageLabel->setWordWrap(true);  // Enable text wrapping
    messageLabel->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    messageLabel->setContentsMargins(8, 6, 8, 6);

    bool isMe =
        message->get_sender_id() == Session::get_instance().authenticated_user.value()->get_uid();

    // Apply styling (optional)
    if (isMe) {
        messageLabel->setStyleSheet(
            "background-color: #0078D7; color: white; border-radius: 10px; padding: 8px;");
    } else {
        messageLabel->setStyleSheet(
            "background-color: #F1F0F0; color: black; border-radius: 10px; padding: 8px;");
    }

    // Delete Button (Only for 'isMe' messages)
    QPushButton* deleteButton = nullptr;
    if (isMe) {
        deleteButton = new QPushButton("X", this);
        deleteButton->setFixedSize(20, 20);
        deleteButton->setStyleSheet("background-color: red; color: white; border-radius: 10px;");
        connect(deleteButton, &QPushButton::clicked, this, &MessageWidget::deleteMessage);
    }

    // Layouts
    QVBoxLayout* messageLayout = new QVBoxLayout();
    messageLayout->addWidget(messageLabel);
    messageLayout->setContentsMargins(0, 0, 0, 0);

    if (isMe) {
        mainLayout->addStretch();  // Push message to right for sent messages
        mainLayout->addLayout(messageLayout);
        if (deleteButton)
            mainLayout->addWidget(deleteButton);
    } else {
        mainLayout->addLayout(messageLayout);
        mainLayout->addStretch();  // Push message to left for received messages
    }

    setLayout(mainLayout);
}

void MessageWidget::deleteMessage() {
    Session& session = Session::get_instance();
    session.tcp_client->delete_message(message);
}

void MessageWidget::paintEvent(QPaintEvent* event) {
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}