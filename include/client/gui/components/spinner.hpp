#pragma once
#include <QLabel>
#include <QMovie>
#include <QWidget>

/**
 * @class Spinner
 * @brief A custom widget that displays a loading spinner animation.
 *
 * The Spinner class is a QWidget that contains a QMovie to display
 * an animated spinner and a QLabel to show the spinner.
 */
class Spinner : public QWidget {
    Q_OBJECT
   public:

    explicit Spinner(QWidget* parent = nullptr);

   private:
    QMovie* spinnerMovie;
    QLabel* spinnerLabel;
};