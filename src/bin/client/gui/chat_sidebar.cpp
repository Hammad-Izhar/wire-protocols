#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>

#include "client/gui/chat_sidebar.hpp"

ChatSidebar::ChatSidebar(QWidget* parent) : QWidget(parent) {
    tabWidget = new QTabWidget(this);

    // Active Chats Tab
    activeChatsTab = new ActiveChatsTab(this);

    // Search Tab
    searchTab = new SearchTab(this, this);

    // Add tabs
    tabWidget->addTab(activeChatsTab, "Chats");
    tabWidget->addTab(searchTab, "Search");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(tabWidget);
    setLayout(mainLayout);

    connect(this, &ChatSidebar::setActiveChatTab, this, &ChatSidebar::onSetActiveChatTab);
}

void ChatSidebar::onSetActiveChatTab(int index) {
    tabWidget->setCurrentIndex(index);
}

void ChatSidebar::reset() {
    activeChatsTab->reset();
    searchTab->reset();
}
