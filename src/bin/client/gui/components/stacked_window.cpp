#include <QApplication>
#include <QWidget>
#include <QStackedWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

#include "client/gui/components/stacked_window.hpp"

StackedWindow::StackedWindow(QWidget *parent) : QWidget(parent)
{
    stackedWidget = new QStackedWidget(this);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(stackedWidget);
    setLayout(mainLayout);
}

void StackedWindow::addWidget(QWidget *widget)
{
    stackedWidget->addWidget(widget);
}

void StackedWindow::setCurrentIndex(int index)
{
    if (index >= 0 && index < stackedWidget->count())
    {
        stackedWidget->setCurrentIndex(index);
    }
}

void StackedWindow::animatePageTransition(int nextIndex)
{
    int currentIndex = stackedWidget->currentIndex();
    if (nextIndex == currentIndex)
        return; // Prevent redundant animation

    QWidget *currentWidget = stackedWidget->currentWidget();
    QWidget *nextWidget = stackedWidget->widget(nextIndex);

    if (!currentWidget || !nextWidget)
    {
        qDebug() << "Invalid widget reference! Transition aborted.";
        return;
    }

    int width = stackedWidget->width();
    int direction = (nextIndex > currentIndex) ? 1 : -1; // Determine slide direction

    // Set initial positions
    nextWidget->move(direction * width, 0);    // Place next widget outside view
    stackedWidget->setCurrentIndex(nextIndex); // Ensure it's visible for animation
    currentWidget->show();

    // Create animations
    QPropertyAnimation *animIn = new QPropertyAnimation(nextWidget, "pos");
    QPropertyAnimation *animOut = new QPropertyAnimation(currentWidget, "pos");

    animIn->setDuration(500);
    animIn->setStartValue(QPoint(direction * width, 0));
    animIn->setEndValue(QPoint(0, 0));
    animIn->setEasingCurve(QEasingCurve::InOutQuad);

    animOut->setDuration(500);
    animOut->setStartValue(QPoint(0, 0));
    animOut->setEndValue(QPoint(-direction * width, 0));
    animOut->setEasingCurve(QEasingCurve::InOutQuad);

    // Run both animations simultaneously
    QParallelAnimationGroup *group = new QParallelAnimationGroup(this);
    group->addAnimation(animIn);
    group->addAnimation(animOut);
    connect(group, &QParallelAnimationGroup::finished, currentWidget, &QWidget::hide);

    group->start();
}
