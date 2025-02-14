#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>

#include <qlistwidget.h>
#include "client/gui/components/profile_widget.hpp"
#include "client/gui/search_tab.hpp"
#include "client/model/session.hpp"

SearchTab::SearchTab(ChatSidebar* sidebar, QWidget* parent) : QWidget(parent), sidebar(sidebar) {
    QVBoxLayout* searchLayout = new QVBoxLayout(this);

    // Horizontal layout for search input and button
    QHBoxLayout* searchBoxLayout = new QHBoxLayout();
    searchField = new QLineEdit();
    searchField->setPlaceholderText("Search...");
    searchField->setStyleSheet("padding: 5px;");
    connect(searchField, &QLineEdit::returnPressed, this, &SearchTab::search);

    QPushButton* searchButton = new QPushButton("Search");
    searchButton->setStyleSheet("padding: 5px 10px;");
    connect(searchButton, &QPushButton::clicked, this, &SearchTab::search);

    searchBoxLayout->addWidget(searchField);
    searchBoxLayout->addWidget(searchButton);

    searchLayout->addLayout(searchBoxLayout);
    searchLayout->addStretch();  // Pushes everything up

    // add a list widget to show search results
    searchResults = new QListWidget();
    searchResults->setSelectionMode(QAbstractItemView::NoSelection);

    searchLayout->addWidget(searchResults);

    // add a spinner
    spinner = new Spinner();
    searchLayout->addWidget(spinner);
    spinner->hide();

    searchLayout->addStretch();  // Pushes everything up

    Session& session = Session::get_instance();
    connect(session.tcp_client, &TcpClient::searchSuccess, this, &SearchTab::onSearchSuccess);
    connect(session.tcp_client, &TcpClient::searchFailure, this, &SearchTab::onSearchFailure);

    this->setLayout(searchLayout);
}

void SearchTab::set_loading(bool loading) {
    searchField->setDisabled(loading);
    loading ? searchResults->hide() : searchResults->show();
    loading ? spinner->show() : spinner->hide();
}

void SearchTab::search() {
    QString query = searchField->text();

    set_loading(true);
    Session& session = Session::get_instance();
    session.tcp_client->search_accounts(query.toStdString());
}

void SearchTab::onSearchSuccess(const std::vector<User::SharedPtr>& accounts) {
    set_loading(false);
    searchResults->clear();
    for (const auto& account : accounts) {
        QListWidgetItem* item = new QListWidgetItem(searchResults);
        ProfileWidget* widget =
            new ProfileWidget(account, qobject_cast<ChatSidebar*>(sidebar), this);
        item->setSizeHint(widget->sizeHint());
        searchResults->setItemWidget(item, widget);

        qDebug() << account->get_username().c_str();
    }
    // TODO: update gui to show search results
}

void SearchTab::onSearchFailure(const QString& errorMessage) {
    set_loading(false);
    qDebug() << "search failed" << errorMessage;
    // TODO: update gui
}