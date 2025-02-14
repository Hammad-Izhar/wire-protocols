#pragma once
#include <qwidget.h>
#include <QStackedWidget>
#include <QWidget>

class StackedWindow : public QWidget {
    Q_OBJECT

   public:
    explicit StackedWindow(QWidget* parent = nullptr);

    void addWidget(QWidget* widget);

    void reset();

    void setCurrentIndex(int index);

    void animatePageTransition(int nextIndex);

   private:
    QStackedWidget* stackedWidget;
};