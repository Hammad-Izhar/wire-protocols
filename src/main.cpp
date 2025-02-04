#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QTextEdit>
#include <QVBoxLayout>

class MyWindow : public QWidget
{
public:
    MyWindow()
    {
        // Create UI elements
        auto *layout = new QVBoxLayout(this);
        auto *label = new QLabel("Hello, Qt 6.8!", this);
        auto *textEdit = new QTextEdit(this);
        auto *button = new QPushButton("Copy Text", this);

        // Set default text in QTextEdit
        textEdit->setPlaceholderText("Type something here...");

        // Add widgets to layout
        layout->addWidget(label);
        layout->addWidget(textEdit);
        layout->addWidget(button);

        // Connect button click to update label with text from textEdit
        connect(button, &QPushButton::clicked, this, [=]()
                { label->setText(textEdit->toPlainText()); });

        // Set window properties
        setLayout(layout);
        setWindowTitle("Qt6.8 Test with Text Area");
        resize(400, 300);
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MyWindow window;
    window.show();
    return app.exec();
}
