#pragma once
#include <QWidget>
#include <QTabWidget>

#include "client/gui/components/profile_widget.hpp"

class ChatSidebar : public QWidget
{
    Q_OBJECT

public:
    explicit ChatSidebar(QWidget *parent = nullptr);

private:
    QTabWidget *tabWidget;
    QWidget *activeChatsTab;
    QWidget *searchTab;
};
