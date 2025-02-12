#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSizePolicy>
#include <QPainter>
#include <QPainterPath>

#include "client/gui/components/profile_widget.hpp"

ProfileWidget::ProfileWidget(const QString &profilePicPath, const QString &displayName, const QString &username, QWidget *parent)
    : QWidget(parent)
{

    // Apply border and padding to the entire widget
    setStyleSheet("border: 2px solid #ccc; border-radius: 10px; background-color: white; padding: 5px;");
    setFixedHeight(70); // Ensure all profile widgets have a uniform height

    // Profile picture
    profilePicLabel = new QLabel(this);

    // Load the image
    QPixmap pixmap(profilePicPath);
    pixmap = pixmap.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // Create a circular mask
    QPixmap circularPixmap(50, 50);
    circularPixmap.fill(Qt::transparent);

    QPainter painter(&circularPixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    // Define a circular region
    QPainterPath path;
    path.addEllipse(0, 0, 50, 50);
    painter.setClipPath(path);

    // Draw the image centered inside the circular mask
    painter.drawPixmap(0, 0, pixmap.width(), pixmap.height(), pixmap);
    painter.end();

    // Apply to QLabel
    profilePicLabel->setPixmap(circularPixmap);
    profilePicLabel->setFixedSize(50, 50);
    profilePicLabel->setAlignment(Qt::AlignCenter);

    // Name and username labels
    displayNameLabel = new QLabel(displayName, this);
    displayNameLabel->setStyleSheet("font-weight: bold; font-size: 14px;");

    usernameLabel = new QLabel("@" + username, this);
    usernameLabel->setStyleSheet("color: gray; font-size: 12px;");

    QVBoxLayout *nameLayout = new QVBoxLayout;
    nameLayout->addWidget(displayNameLabel);
    nameLayout->addWidget(usernameLabel);
    nameLayout->setSpacing(2);

    // Message button
    messageButton = new QPushButton("Message", this);
    messageButton->setStyleSheet("padding: 5px 10px;");
    messageButton->setStyleSheet("QPushButton { background-color: #007BFF; color: white; border: none; border-radius: 5px; }"
                                 "QPushButton:hover { background-color: #0056b3; }"
                                 "QPushButton:pressed { background-color: #004085; }");

    // Main layout with padding
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(profilePicLabel);
    mainLayout->addLayout(nameLayout);
    mainLayout->addStretch();
    mainLayout->addWidget(messageButton);
    mainLayout->setContentsMargins(10, 5, 10, 5); // Padding inside the border

    setLayout(mainLayout);
}