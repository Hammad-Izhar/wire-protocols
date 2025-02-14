#pragma once
#include <QLineEdit>
#include <QString>
#include <QValidator>
#include <QWidget>

/**
 * @class ValidatedTextInput
 * @brief A custom QWidget that provides a text input field with validation and additional functionality.
 *
 * This class extends QWidget to create a text input field that can be validated using a QValidator.
 * It also supports custom actions when the text changes or when the return key is pressed.
 *
 * @param parent The parent widget of this widget. Defaults to nullptr.
 * @param objectName The object name for this widget. Defaults to an empty string.
 * @param placeholder The placeholder text for the input field. Defaults to an empty string.
 * @param validator A pointer to a QValidator object used to validate the input. Defaults to nullptr.
 * @param echoMode The echo mode for the input field (e.g., normal, password). Defaults to QLineEdit::EchoMode::Normal.
 * @param on_text_changed A function to be called when the text in the input field changes. Defaults to nullptr.
 * @param on_return_pressed A function to be called when the return key is pressed. Defaults to nullptr.
 */
class ValidatedTextInput : public QWidget {
    Q_OBJECT
   public:
    explicit ValidatedTextInput(
        QWidget* parent = nullptr,
        const QString& objectName = "",
        const QString& placeholder = "",
        const QValidator* validator = nullptr,
        const QLineEdit::EchoMode = QLineEdit::EchoMode::Normal,
        std::function<bool(const QString&, QWidget*)> on_text_changed = nullptr,
        std::function<void()> on_return_pressed = nullptr);

   private:
    QLineEdit* input;
};