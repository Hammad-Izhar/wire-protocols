#include "client/gui/components/channel_widget.hpp"
#include <QVBoxLayout>

ChannelWidget::ChannelWidget(const Channel::SharedPtr& channel, QWidget* parent)
    : QWidget(parent), channel(channel) {
    channelNameLabel = new QLabel(QString::fromStdString(channel->get_name()), this);
    channelNameLabel->setStyleSheet("font-weight: bold; font-size: 14px;");

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(channelNameLabel);
    setLayout(layout);
}

Channel::SharedPtr ChannelWidget::get_channel() const {
    return channel;
}