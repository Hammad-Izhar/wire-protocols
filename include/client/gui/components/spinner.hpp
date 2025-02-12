#pragma once
#include <QLabel>
#include <QMovie>
#include <QWidget>

class Spinner : public QWidget {
    Q_OBJECT
   public:
    explicit Spinner(QWidget* parent = nullptr);

   private:
    QMovie* spinnerMovie;
    QLabel* spinnerLabel;
};