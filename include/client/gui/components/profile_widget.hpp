#pragma once
#include <QLabel>
#include <QPushButton>
#include <QWidget>

#include "client/gui/chat_sidebar.hpp"
#include "models/user.hpp"

class ProfileWidget : public QWidget {
    Q_OBJECT

   public:
    explicit ProfileWidget(const User::SharedPtr& user,
                           ChatSidebar* sidebar,
                           QWidget* parent = nullptr);

   private:
    const User::SharedPtr user;
    ChatSidebar* sidebar;

    QLabel* profilePicLabel;
    QLabel* displayNameLabel;
    QLabel* usernameLabel;
    QPushButton* messageButton;

   private slots:
    void onMessageButtonClicked();
};
