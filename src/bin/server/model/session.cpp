#include <iostream>
#include <optional>
#ifdef PROTOCOL_RPC
#include "server/model/session.hpp"

Session& Session::get_instance(std::optional<std::string> hostname, std::optional<uint16_t> port) {
    static Session instance(hostname.value_or(""),
                            port.value_or(0));  // Default to 0 if no port is provided
    assert(instance.port != 0);
    assert(instance.hostname != "");

    return instance;
}

void Session::save_message_stream(const std::string& username,
                                  grpc::ServerWriter<socketout::MessageResponse>* response) {
    message_streams[username] = response;
}

void Session::save_channel_stream(const std::string& username,
                                  grpc::ServerWriter<socketout::ChannelResponse>* response) {
    channel_streams[username] = response;
}

void Session::attach(const std::string hostname, const uint16_t port) {
    std::cout << "Added replica at: " << hostname << ":" << port << std::endl;
    if (port != 0) {
        replicas.insert(std::make_pair(hostname, port));
    }
}

std::optional<grpc::ServerWriter<socketout::MessageResponse>*> Session::get_message_stream(
    const std::string& username) {
    if (message_streams.find(username) == message_streams.end()) {
        return std::nullopt;
    }
    return message_streams[username];
}

std::optional<grpc::ServerWriter<socketout::ChannelResponse>*> Session::get_channel_stream(
    const std::string& username) {
    if (channel_streams.find(username) == channel_streams.end()) {
        return std::nullopt;
    }
    return channel_streams[username];
}

void Session::remove_message_stream(const std::string& username) {
    message_streams.erase(username);
}

void Session::remove_channel_stream(const std::string& username) {
    channel_streams.erase(username);
}
#endif
