#pragma once
#include <cstdint>
#include <utility>
#include <string>
#include <optional>
#include <QMutex>
#include <QWaitCondition>

#include "client/model/TcpClient.hpp"

class Session
{
public:
    std::optional<std::pair<std::string, uint16_t>> connection_address;
    std::optional<std::string> error;
    std::unique_ptr<TcpClient> tcp_client;
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