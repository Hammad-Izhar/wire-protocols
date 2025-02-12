#pragma once
#include <QLineEdit>
#include <QString>
#include <QValidator>
#include <QWidget>

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