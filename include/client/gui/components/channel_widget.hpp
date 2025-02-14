#pragma once
#include <QLabel>
#include <QWidget>

#include "models/channel.hpp"


/**
 * @class ChannelWidget
 * @brief A widget that represents a communication channel in the GUI.
 */
class ChannelWidget : public QWidget {
    Q_OBJECT

   public:
    /**
     * @brief Constructs a ChannelWidget.
     * @param channel A shared pointer to the Channel object.
     * @param parent The parent widget, default is nullptr.
     */
    explicit ChannelWidget(const Channel::SharedPtr& channel, QWidget* parent = nullptr);

    /**
     * @brief Retrieves the associated Channel object.
     * @return A shared pointer to the Channel object.
     */
    Channel::SharedPtr get_channel() const;

   private:
    const Channel::SharedPtr channel; ///< The associated Channel object.
    QLabel* channelNameLabel; ///< Label to display the channel name.
};
