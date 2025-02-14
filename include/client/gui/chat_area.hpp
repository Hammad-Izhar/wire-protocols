#pragma once
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>

class ChatArea : public QWidget {
    Q_OBJECT

   public:
    explicit ChatArea(QWidget* parent = nullptr);

   private slots:
    void validateMessage();
    void onActiveChannelChanged();
    void sendMessage();

   private:
    QLabel* chatTitle;
    QScrollArea* messageScrollArea;
    QWidget* messageContainer;
    QVBoxLayout* messageLayout;
    QLineEdit* messageInput;
    QPushButton* sendButton;
};
