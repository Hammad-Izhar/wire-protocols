#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>

#include "client/gui/chat_sidebar.hpp"
#include "client/gui/search_tab.hpp"

ChatSidebar::ChatSidebar(QWidget* parent) : QWidget(parent) {
    tabWidget = new QTabWidget(this);

    // Active Chats Tab
    activeChatsTab = new QWidget();
    QVBoxLayout* activeChatsLayout = new QVBoxLayout(activeChatsTab);

    QScrollArea* scrollArea = new QScrollArea();
    QWidget* chatListWidget = new QWidget();
    QVBoxLayout* chatListLayout = new QVBoxLayout(chatListWidget);

    chatListWidget->setLayout(chatListLayout);
    scrollArea->setWidget(chatListWidget);
    scrollArea->setWidgetResizable(true);

    activeChatsLayout->addWidget(scrollArea);
    activeChatsTab->setLayout(activeChatsLayout);

    // Search Tab
    searchTab = new SearchTab(this);

    // Add tabs
    tabWidget->addTab(activeChatsTab, "Chats");
    tabWidget->addTab(searchTab, "Search");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(tabWidget);
    setLayout(mainLayout);
}
