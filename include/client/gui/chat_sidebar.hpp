#pragma once
#include <QTabWidget>
#include <QWidget>

class ChatSidebar : public QWidget {
    Q_OBJECT

   public:
    explicit ChatSidebar(QWidget* parent = nullptr);

   private:
    QTabWidget* tabWidget;
    QWidget* activeChatsTab;
    QWidget* searchTab;
};
