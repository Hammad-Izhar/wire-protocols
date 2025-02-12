#include "client/model/message_handlers.hpp"
#include "client/model/session.hpp"
#include "message/login_response.hpp"
#include "message/register_account_response.hpp"

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