#pragma once

#include <sys/types.h>
#include <cstdint>
#ifdef PROTOCOL_RPC
#include <grpcpp/support/sync_stream.h>
#include <optional>
#include <string>
#include <unordered_set>

#include "socketout.pb.h"

class Session {
    Session(uint16_t port) : port(port) {}

   public:
    static Session& get_instance(std::optional<uint16_t> port = std::nullopt);

    void save_message_stream(const std::string& username,
                             grpc::ServerWriter<socketout::MessageResponse>* response);

    void save_channel_stream(const std::string& username,
                             grpc::ServerWriter<socketout::ChannelResponse>* response);

    uint16_t get_port() const { return port; }

    std::unordered_set<uint16_t> get_replicas() const { return replica_ports; }

    void attach(const uint16_t port);

    std::optional<grpc::ServerWriter<socketout::MessageResponse>*> get_message_stream(
        const std::string& username);

    std::optional<grpc::ServerWriter<socketout::ChannelResponse>*> get_channel_stream(
        const std::string& username);

    void remove_message_stream(const std::string& username);

    void remove_channel_stream(const std::string& username);

   private:
    uint16_t port;
    std::unordered_map<std::string, grpc::ServerWriter<socketout::MessageResponse>*>
        message_streams;
    std::unordered_map<std::string, grpc::ServerWriter<socketout::ChannelResponse>*>
        channel_streams;
    std::unordered_set<uint16_t> replica_ports;
};
#endif
