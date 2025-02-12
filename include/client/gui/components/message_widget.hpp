#pragma once
#include <QWidget>

class MessageWidget : public QWidget {
    Q_OBJECT

   public:
    explicit MessageWidget(const QString& message, bool isMe, QWidget* parent = nullptr);
};