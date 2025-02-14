#pragma once
#include <QLabel>
#include <QPushButton>
#include <QWidget>

#include "models/user.hpp"

class ProfileWidget : public QWidget {
    Q_OBJECT

   public:
    explicit ProfileWidget(const User::SharedPtr& user, QWidget* parent = nullptr);

   private:
    const User::SharedPtr user;
    QLabel* profilePicLabel;
    QLabel* displayNameLabel;
    QLabel* usernameLabel;
    QPushButton* messageButton;

   private slots:
    void onMessageButtonClicked();
};
