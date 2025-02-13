#include "client/gui/search_tab.hpp"
#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>

#include "client/model/session.hpp"

SearchTab::SearchTab(QWidget* parent) : QWidget(parent) {
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

    // add a spinner
    spinner = new Spinner();
    searchLayout->addWidget(spinner);
    spinner->hide();

    searchLayout->addStretch();  // Pushes everything up

    Session& session = Session::getInstance();
    connect(session.tcp_client, &TcpClient::searchSuccess, this, &SearchTab::onSearchSuccess);
    connect(session.tcp_client, &TcpClient::searchFailure, this, &SearchTab::onSearchFailure);

    this->setLayout(searchLayout);
}

void SearchTab::set_loading(bool loading) {
    searchField->setDisabled(loading);
    loading ? spinner->show() : spinner->hide();
}

void SearchTab::search() {
    QString query = searchField->text();

    set_loading(true);
    Session& session = Session::getInstance();
    session.tcp_client->search_accounts(query.toStdString());
}

void SearchTab::onSearchSuccess(const std::vector<User::SharedPtr>& accounts) {
    set_loading(true);
    for (const auto& account : accounts) {
        qDebug() << account->get_username().c_str();
    }
    // TODO: update gui to show search results
}

void SearchTab::onSearchFailure(const QString& errorMessage) {
    set_loading(true);
    qDebug() << "search failed" << errorMessage;
    // TODO: update gui
}