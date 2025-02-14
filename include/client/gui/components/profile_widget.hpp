#pragma once
#include <QLabel>
#include <QPushButton>
#include <QWidget>

#include "client/gui/chat_sidebar.hpp"
#include "models/user.hpp"

/**
 * @class ProfileWidget
 * @brief A widget that displays a user's profile information and provides an interface for messaging.
 */
class ProfileWidget : public QWidget {
    Q_OBJECT

   public:
    /**
     * @brief Constructs a ProfileWidget.
     * @param user A shared pointer to the User object.
     * @param sidebar A pointer to the ChatSidebar.
     * @param parent The parent widget, default is nullptr.
     */
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

    /**
    * @brief Slot function that handles the message button click event.
    */
   private slots:
    void onMessageButtonClicked();
};
