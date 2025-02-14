#pragma once
#include <QLabel>
#include <QWidget>

#include "models/channel.hpp"

class ChannelWidget : public QWidget {
    Q_OBJECT

   public:
    explicit ChannelWidget(const Channel::SharedPtr& channel, QWidget* parent = nullptr);

    Channel::SharedPtr get_channel() const;

   private:
    const Channel::SharedPtr channel;
    QLabel* channelNameLabel;
};
