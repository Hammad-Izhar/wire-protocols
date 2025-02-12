#include <QVBoxLayout>

#include "client/gui/components/spinner.hpp"

Spinner::Spinner(QWidget* parent) : QWidget(parent) {
    spinnerMovie = new QMovie(":/assets/animations/loading.gif");
    spinnerMovie->setScaledSize(QSize(32, 32));  // Scale GIF to match label size
    spinnerLabel = new QLabel(this);
    spinnerLabel->setFixedSize(32, 32);  // Set small fixed size
    spinnerLabel->setMovie(spinnerMovie);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(spinnerLabel);
    layout->setAlignment(Qt::AlignCenter);  // Center spinner in widget

    spinnerMovie->start();
}