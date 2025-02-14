#pragma once
#include <QObject>
#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>

#include "client/gui/components/stacked_window.hpp"
#include "client/model/tcp_client.hpp"
#include "models/uuid.hpp"

enum Window { CONNECTION = 0, AUTHENTICATION = 1, MAIN = 2 };

class Session : public QObject {
    Q_OBJECT

   public:
    std::optional<std::pair<std::string, uint16_t>> connection_address;
    std::optional<User::SharedPtr> authenticated_user;
    std::unordered_map<UUID, Channel::SharedPtr> channels;

    TcpClient* tcp_client;
    StackedWindow* main_window;

    static Session& get_instance();

    std::optional<Channel::SharedPtr> get_active_channel() const;

    void set_active_channel(const Channel::SharedPtr& channel);

   private:
    std::optional<Channel::SharedPtr> open_channel;

    Session() = default;
    ~Session() = default;

    Session(const Session&) = delete;
    Session& operator=(const Session&) = delete;
    Session(Session&&) = delete;
    Session& operator=(Session&&) = delete;

   signals:
    void updateActiveChannel();
};