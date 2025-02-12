#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>

#include "client/gui/chat_sidebar.hpp"
#include "client/gui/components/profile_widget.hpp"

ChatSidebar::ChatSidebar(QWidget* parent) : QWidget(parent) {
    tabWidget = new QTabWidget(this);

    // Active Chats Tab
    activeChatsTab = new QWidget();
    QVBoxLayout* activeChatsLayout = new QVBoxLayout(activeChatsTab);

    QScrollArea* scrollArea = new QScrollArea();
    QWidget* chatListWidget = new QWidget();
    QVBoxLayout* chatListLayout = new QVBoxLayout(chatListWidget);

    // Example profiles
    chatListLayout->addWidget(new ProfileWidget(":/assets/profile_pics/blank_profile_pic.png",
                                                "Alice Johnson", "alicej"));
    chatListLayout->addWidget(
        new ProfileWidget(":/assets/profile_pics/blank_profile_pic.png", "Bob Smith", "bobsmith"));
    chatListLayout->addWidget(new ProfileWidget(":/assets/profile_pics/blank_profile_pic.png",
                                                "Alice Johnson", "alicej"));
    chatListLayout->addWidget(
        new ProfileWidget(":/assets/profile_pics/blank_profile_pic.png", "Bob Smith", "bobsmith"));
    chatListLayout->addWidget(new ProfileWidget(":/assets/profile_pics/blank_profile_pic.png",
                                                "Alice Johnson", "alicej"));
    chatListLayout->addWidget(
        new ProfileWidget(":/assets/profile_pics/blank_profile_pic.png", "Bob Smith", "bobsmith"));

    chatListWidget->setLayout(chatListLayout);
    scrollArea->setWidget(chatListWidget);
    scrollArea->setWidgetResizable(true);

    activeChatsLayout->addWidget(scrollArea);
    activeChatsTab->setLayout(activeChatsLayout);

    // Search Tab
    searchTab = new QWidget();
    QVBoxLayout* searchLayout = new QVBoxLayout(searchTab);

    // Horizontal layout for search input and button
    QHBoxLayout* searchBoxLayout = new QHBoxLayout();
    QLineEdit* searchField = new QLineEdit();
    searchField->setPlaceholderText("Search...");
    searchField->setStyleSheet("padding: 5px;");

    QPushButton* searchButton = new QPushButton("Search");
    searchButton->setStyleSheet("padding: 5px 10px;");

    searchBoxLayout->addWidget(searchField);
    searchBoxLayout->addWidget(searchButton);

    searchLayout->addLayout(searchBoxLayout);
    searchLayout->addStretch();  // Pushes everything up

    searchTab->setLayout(searchLayout);

    // Add tabs
    tabWidget->addTab(activeChatsTab, "Chats");
    tabWidget->addTab(searchTab, "Search");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(tabWidget);
    setLayout(mainLayout);
}
