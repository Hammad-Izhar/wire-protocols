#pragma once
#include <qtmetamacros.h>
#include <QWidget>

#include "models/message.hpp"

class MessageWidget : public QWidget {
    Q_OBJECT

   public:
    explicit MessageWidget(Message::SharedPtr message, QWidget* parent = nullptr);

   protected:
    void paintEvent(QPaintEvent* event) override;

   private:
    Message::SharedPtr message;

   private slots:
    void deleteMessage();
};