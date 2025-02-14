#pragma once
#include <QLineEdit>
#include <QListWidget>
#include <QWidget>

#include "client/gui/components/spinner.hpp"
#include "models/user.hpp"

class ChatSidebar;
class SearchTab : public QWidget {
    Q_OBJECT

   public:
    explicit SearchTab(ChatSidebar* sidebar, QWidget* parent = nullptr);

   private:
    ChatSidebar* sidebar;
    QLineEdit* searchField;
    QListWidget* searchResults;
    Spinner* spinner;
    void set_loading(bool loading);

   private slots:
    void search();
    void onSearchSuccess(const std::vector<User::SharedPtr>& accounts);
    void onSearchFailure(const QString& errorMessage);
};