#pragma once
#include <cstdint>
#include <utility>
#include <string>
#include <optional>
#include <QMutex>
#include <QWaitCondition>

class Session
{
public:
    QWaitCondition gui_cond_var;
    QWaitCondition worker_cond_var;
    std::optional<std::string> error;
    std::optional<std::pair<std::string, uint16_t>> connection_address;
    QMutex session_mutex;
    std::optional<int> socket_fd;

    static Session &getInstance();

private:
    Session() = default;
    ~Session() = default;

    Session(const Session &) = delete;
    Session &operator=(const Session &) = delete;
    Session(Session &&) = delete;
    Session &operator=(Session &&) = delete;
};