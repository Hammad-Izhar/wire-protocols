#pragma once
#include <QWidget>
#include <QStackedWidget>

class StackedWindow : public QWidget
{
    Q_OBJECT

public:
    explicit StackedWindow(QWidget *parent = nullptr);

    void addWidget(QWidget *widget);

    void setCurrentIndex(int index);

    void animatePageTransition(int nextIndex);

private:
    QStackedWidget *stackedWidget;
};