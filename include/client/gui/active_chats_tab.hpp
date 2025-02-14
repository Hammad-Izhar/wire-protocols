#pragma once
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QWidget>

#include "models/channel.hpp"

class ActiveChatsTab : public QWidget {
    Q_OBJECT

   public:
    explicit ActiveChatsTab(QWidget* parent = nullptr);

    void reset();

   private:
    QListWidget* activeChats;

   private slots:
    void onCreateChannelSuccess(Channel::SharedPtr channel);
    void onCreateChannelFailure(const QString& errorMessage);
    void onChatSelected(QListWidgetItem* item);
};