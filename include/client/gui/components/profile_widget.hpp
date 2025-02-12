#pragma once
#include <QLabel>
#include <QPushButton>
#include <QWidget>

class ProfileWidget : public QWidget {
    Q_OBJECT

   public:
    explicit ProfileWidget(const QString& profilePicPath,
                           const QString& displayName,
                           const QString& username,
                           QWidget* parent = nullptr);

   private:
    QLabel* profilePicLabel;
    QLabel* displayNameLabel;
    QLabel* usernameLabel;
    QPushButton* messageButton;
};
