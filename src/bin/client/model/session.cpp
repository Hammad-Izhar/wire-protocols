#include <QApplication>

#include "client/model/session.hpp"

Session &Session::getInstance()
{
    static Session instance;
    instance.tcp_client = std::make_unique<TcpClient>(QApplication::instance());
    return instance;
}