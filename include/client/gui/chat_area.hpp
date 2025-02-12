#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QLineEdit>
#include <QPushButton>

#include "client/gui/components/message_widget.hpp"

class ChatArea : public QWidget
{
    Q_OBJECT

public:
    explicit ChatArea(QWidget *parent = nullptr);

private slots:
    void validateMessage();

private:
    QLabel *chatTitle;
    QScrollArea *messageScrollArea;
    QWidget *messageContainer;
    QVBoxLayout *messageLayout;
    QLineEdit *messageInput;
    QPushButton *sendButton;
};
