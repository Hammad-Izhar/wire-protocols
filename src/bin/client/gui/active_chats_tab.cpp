#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>

#include <qlistwidget.h>
#include "client/gui/active_chats_tab.hpp"
#include "client/gui/components/channel_widget.hpp"
#include "client/model/session.hpp"

ActiveChatsTab::ActiveChatsTab(QWidget* parent) : QWidget(parent) {
    QVBoxLayout* searchLayout = new QVBoxLayout(this);

    activeChats = new QListWidget();
    connect(activeChats, &QListWidget::itemClicked, this, &ActiveChatsTab::onChatSelected);

    searchLayout->addWidget(activeChats);

    Session& session = Session::get_instance();
    connect(session.tcp_client, &TcpClient::createChannelSuccess, this,
            &ActiveChatsTab::onCreateChannelSuccess);
    connect(session.tcp_client, &TcpClient::createChannelFailure, this,
            &ActiveChatsTab::onCreateChannelFailure);

    this->setLayout(searchLayout);
}

void ActiveChatsTab::onCreateChannelSuccess(Channel::SharedPtr channel) {
    ChannelWidget* channelWidget = new ChannelWidget(channel);
    QListWidgetItem* item = new QListWidgetItem(activeChats);
    item->setSizeHint(channelWidget->sizeHint());
    activeChats->setItemWidget(activeChats->item(activeChats->count() - 1), channelWidget);

    activeChats->setCurrentItem(activeChats->item(activeChats->count() - 1));
}

void ActiveChatsTab::onChatSelected(QListWidgetItem* item) {
    Session& session = Session::get_instance();
    ChannelWidget* channelWidget = qobject_cast<ChannelWidget*>(activeChats->itemWidget(item));
    session.set_active_channel(channelWidget->get_channel());
}

void ActiveChatsTab::onCreateChannelFailure(const QString& errorMessage) {
    qDebug() << errorMessage;
    // TODO: show error message
}

void ActiveChatsTab::reset() {
    activeChats->clear();
}