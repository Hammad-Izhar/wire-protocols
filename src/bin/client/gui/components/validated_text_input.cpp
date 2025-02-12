#include <QHBoxLayout>

#include "client/gui/components/validated_text_input.hpp"

ValidatedTextInput::ValidatedTextInput(
    QWidget* parent,
    const QString& objectName,
    const QString& placeholder,
    const QValidator* validator,
    const QLineEdit::EchoMode echoMode,
    std::function<bool(const QString&, QWidget*)> on_text_changed,
    std::function<void()> on_return_pressed)
    : QWidget(parent) {
    input = new QLineEdit(this);
    input->setObjectName(objectName);
    input->setPlaceholderText(placeholder);
    input->setValidator(validator);
    input->setEchoMode(echoMode);

    if (on_text_changed) {
        connect(input, &QLineEdit::textChanged, this,
                std::bind(on_text_changed, std::placeholders::_1, input));
    };

    if (on_return_pressed) {
        connect(input, &QLineEdit::returnPressed, this, on_return_pressed);
    }

    QBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(input);
}