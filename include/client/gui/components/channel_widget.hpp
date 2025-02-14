/**
 * @file channel_widget.hpp
 * @brief Declaration of the ChannelWidget class for the GUI component representing a channel.
 */

 /**
    * @class ChannelWidget
    * @brief A QWidget-based class that represents a channel in the GUI.
    *
    * This class provides a widget that displays information about a channel.
    */
 
 /**
    * @brief Constructs a ChannelWidget.
    * 
    * @param channel A shared pointer to the Channel object to be represented.
    * @param parent The parent widget, default is nullptr.
    */
 
 /**
    * @brief Retrieves the channel associated with this widget.
    * 
    * @return A shared pointer to the Channel object.
    */
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
