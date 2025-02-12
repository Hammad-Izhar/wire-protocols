#pragma once
#include <QWidget>
#include <QLabel>
#include <QPushButton>

class ProfileWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ProfileWidget(const QString &profilePicPath, const QString &displayName, const QString &username, QWidget *parent = nullptr);

private:
    QLabel *profilePicLabel;
    QLabel *displayNameLabel;
    QLabel *usernameLabel;
    QPushButton *messageButton;
};
