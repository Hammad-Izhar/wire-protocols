#pragma once
#include <qtmetamacros.h>
#include <QWidget>

#include "models/message.hpp"

/**
 * @class MessageWidget
 * @brief A widget for displaying and interacting with a message.
 */
class MessageWidget : public QWidget {
    Q_OBJECT

   public:
    /**
     * @brief Constructs a MessageWidget.
     * @param message The message to be displayed.
     * @param parent The parent widget.
     */
    explicit MessageWidget(Message::SharedPtr message, QWidget* parent = nullptr);

    /**
     * @brief Retrieves the message associated with this widget.
     * @return The message associated with this widget.
     */
    Message::SharedPtr getMessage() const { return message; }

   protected:
    /**
     * @brief Handles the paint event for the widget.
     * @param event The paint event.
     */
    void paintEvent(QPaintEvent* event) override;

   private:
    Message::SharedPtr message;

   private slots:
    /**
     * @brief Slot to delete the message.
     */
    void deleteMessage();
};