#include <QHBoxLayout>
#include <QLabel>

#include "client/gui/components/message_widget.hpp"

MessageWidget::MessageWidget(const QString& message, bool isMe, QWidget* parent) : QWidget(parent) {
    QHBoxLayout* layout = new QHBoxLayout(this);

    QLabel* messageLabel = new QLabel(message, this);
    messageLabel->setStyleSheet("border-radius: 10px; padding: 5px;");
    messageLabel->setWordWrap(true);
    messageLabel->setFixedHeight(30);  // Fixed height for messages

    if (isMe) {
        messageLabel->setStyleSheet(
            "background-color: lightblue; border-radius: 10px; padding: 5px;");
        layout->addStretch();
        layout->addWidget(messageLabel);
    } else {
        messageLabel->setStyleSheet(
            "background-color: lightgray; border-radius: 10px; padding: 5px;");
        layout->addWidget(messageLabel);
        layout->addStretch();
    }

    setLayout(layout);
}