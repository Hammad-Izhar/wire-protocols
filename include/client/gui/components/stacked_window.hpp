#pragma once
#include <qwidget.h>
#include <QStackedWidget>
#include <QWidget>

/**
 * @class StackedWindow
 * @brief A custom widget that manages a stack of widgets with transition animations.
 *
 * The StackedWindow class provides a container for stacking multiple widgets
 * and allows for animated transitions between them.
 */
class StackedWindow : public QWidget {
    Q_OBJECT

   public:
    /**
     * @brief Constructs a StackedWindow object.
     * @param parent The parent widget. Defaults to nullptr.
     */
    explicit StackedWindow(QWidget* parent = nullptr);

    /**
     * @brief Adds a widget to the stack.
     * @param widget The widget to be added.
     */
    void addWidget(QWidget* widget);

    /**
     * @brief Resets the stacked widget, removing all widgets.
     */
    void reset();

    /**
     * @brief Sets the current index of the stacked widget.
     * @param index The index to be set as current.
     */
    void setCurrentIndex(int index);

    /**
     * @brief Animates the transition to the specified page.
     * @param nextIndex The index of the next page to transition to.
     */
    void animatePageTransition(int nextIndex);

   private:
    QStackedWidget* stackedWidget; ///< The stacked widget that manages the pages.
};