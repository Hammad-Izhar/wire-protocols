#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpacerItem>
#include <QVBoxLayout>

#include "client/gui/chat_window.hpp"
#include "client/model/session.hpp"

ChatWindow::ChatWindow(QWidget* parent) : QWidget(parent) {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Top Bar Layout
    QHBoxLayout* topBarLayout = new QHBoxLayout();

    QLabel* chatTitle = new QLabel("Chat", this);  // Chat title label
    QPushButton* disconnectButton = new QPushButton("Disconnect", this);
    QPushButton* deleteAccountButton = new QPushButton("Delete Account", this);

    // Style buttons (optional)
    disconnectButton->setStyleSheet("background-color: orange; padding: 5px;");
    deleteAccountButton->setStyleSheet("background-color: red; color: white; padding: 5px;");

    // Add spacer to push buttons to the right
    QSpacerItem* spacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    // Add widgets to the top bar layout
    topBarLayout->addWidget(chatTitle);
    topBarLayout->addItem(spacer);
    topBarLayout->addWidget(disconnectButton);
    topBarLayout->addWidget(deleteAccountButton);

    // Main Layout
    QHBoxLayout* chatLayout = new QHBoxLayout();
    sidebar = new ChatSidebar(this);
    chatArea = new ChatArea(this);
    chatLayout->addWidget(sidebar);
    chatLayout->addWidget(chatArea, 1);

    // Combine layouts
    mainLayout->addLayout(topBarLayout);
    mainLayout->addLayout(chatLayout, 1);

    setLayout(mainLayout);

    // Connect buttons to actions (Replace with actual slot functions)
    connect(disconnectButton, &QPushButton::clicked, this, &ChatWindow::onDisconnect);
    connect(deleteAccountButton, &QPushButton::clicked, this, &ChatWindow::onDeleteAccount);
}

// Placeholder slot functions (Define in .hpp and implement in .cpp)
void ChatWindow::onDisconnect() {
    // Implement disconnect logic
    Session& session = Session::get_instance();

    session.tcp_client->disconnectFromServer();
    session.reset();
}

void ChatWindow::onDeleteAccount() {
    QDialog dialog(this);
    dialog.setWindowTitle("Delete Account");

    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    QFormLayout* formLayout = new QFormLayout();

    // Input fields
    QLineEdit* usernameInput = new QLineEdit(&dialog);
    QLineEdit* passwordInput = new QLineEdit(&dialog);
    passwordInput->setEchoMode(QLineEdit::Password);  // Hide password input

    formLayout->addRow("Username:", usernameInput);
    formLayout->addRow("Password:", passwordInput);

    layout->addLayout(formLayout);

    // Buttons (Cancel, Delete)
    QDialogButtonBox* buttonBox =
        new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Ok, &dialog);
    layout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, [&]() {
        QString username = usernameInput->text();
        QString password = passwordInput->text();

        if (username.isEmpty() || password.isEmpty()) {
            QMessageBox::warning(&dialog, "Error", "Please enter both username and password.");
            return;
        }

        Session::get_instance().tcp_client->delete_account(username.toStdString(),
                                                           password.toStdString());
        dialog.accept();
    });

    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    dialog.exec();
}

void ChatWindow::reset() {
    sidebar->reset();
    chatArea->reset();
}