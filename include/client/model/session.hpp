#pragma once
#include <cstdint>
#include <utility>
#include <string>
#include <optional>
#include <QMutex>
#include <QWaitCondition>

#include "client/model/tcp_client.hpp"
#include "client/gui/components/stacked_window.hpp"

enum Window
{
    CONNECTION = 0,
    AUTHENTICATION = 1,
    MAIN = 2
};

class Session
{
public:
    std::optional<std::pair<std::string, uint16_t>> connection_address;
    std::optional<std::string> error;
    TcpClient *tcp_client;
    StackedWindow *main_window;
    QMutex session_mutex;

    static Session &getInstance();

private:
    Session() = default;
    ~Session() = default;

    Session(const Session &) = delete;
    Session &operator=(const Session &) = delete;
    Session(Session &&) = delete;
    Session &operator=(Session &&) = delete;
};