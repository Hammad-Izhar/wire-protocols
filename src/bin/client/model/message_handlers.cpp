#include "client/model/message_handlers.hpp"
#include "client/model/session.hpp"
#include "message/delete_account_response.hpp"
#include "message/list_accounts_response.hpp"
#include "message/login_response.hpp"
#include "message/register_account_response.hpp"
#include "models/message_handler.hpp"

void on_register_account_response(QTcpSocket* socket, RegisterAccountResponse& msg) {
    Session& session = Session::getInstance();
    if (msg.is_success()) {
        emit session.tcp_client->registrationSuccess();
    } else {
        emit session.tcp_client->registrationFailure(
            QString::fromStdString(msg.get_error_message().value()));
    }
};

void on_login_response(QTcpSocket* socket, LoginResponse& msg) {
    Session& session = Session::getInstance();
    if (msg.is_success()) {
        User::SharedPtr usr = msg.get_data().value();
        session.authenticated_user = usr;
        qDebug() << "Authenticated user: " << QString::fromStdString(usr->get_username());
        qDebug() << "Authenticated display name: "
                 << QString::fromStdString(usr->get_display_name());
        qDebug() << "Authenticated profile pic: " << QString::fromStdString(usr->get_profile_pic());

        emit session.tcp_client->loginSuccess();
    } else {
        emit session.tcp_client->loginFailure(
            QString::fromStdString(msg.get_error_message().value()));
    }
};

void on_list_accounts_response(QTcpSocket* socket, ListAccountsResponse& msg) {
    Session& session = Session::getInstance();
    if (msg.is_success()) {
        emit session.tcp_client->searchSuccess(msg.get_users().value());
    } else {
        emit session.tcp_client->searchFailure(
            QString::fromStdString(msg.get_error_message().value()));
    }
};

void on_delete_account_response(QTcpSocket* socket, DeleteAccountResponse& msg) {
    Session& session = Session::getInstance();
    if (msg.is_success()) {
        session.authenticated_user.reset();
        session.main_window->animatePageTransition(Window::AUTHENTICATION);
        emit session.tcp_client->deleteAccountSuccess();
    } else {
        emit session.tcp_client->deleteAccountFailure(
            QString::fromStdString(msg.get_error_message().value()));
    }
};

void init_message_handlers() {
    MessageHandler& messageHandler = MessageHandler::get_instance();
    messageHandler.register_handler<RegisterAccountResponse>(&on_register_account_response);
    messageHandler.register_handler<LoginResponse>(&on_login_response);
    messageHandler.register_handler<ListAccountsResponse>(&on_list_accounts_response);
    messageHandler.register_handler<DeleteAccountResponse>(&on_delete_account_response);
}