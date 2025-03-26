#pragma once

#include <sys/types.h>
#include <cstdint>
#ifdef PROTOCOL_RPC
#include <grpcpp/support/sync_stream.h>
#include <optional>
#include <string>
#include <unordered_set>

#include "socketout.pb.h"

namespace std {
template <>
struct hash<std::pair<std::string, uint16_t>> {
    size_t operator()(const std::pair<std::string, uint16_t>& p) const {
        size_t h1 = std::hash<std::string>{}(p.first);
        size_t h2 = std::hash<uint16_t>{}(p.second);
        // Combine the two hash values (simple combination)
        return h1 ^ (h2 << 1);
    }
};
}  // namespace std

class Session {
    Session(std::string hostname, uint16_t port) : hostname(hostname), port(port) {}

   public:
    static Session& get_instance(std::optional<std::string> hostname = std::nullopt,
                                 std::optional<uint16_t> port = std::nullopt);

    void save_message_stream(const std::string& username,
                             grpc::ServerWriter<socketout::MessageResponse>* response);

    void save_channel_stream(const std::string& username,
                             grpc::ServerWriter<socketout::ChannelResponse>* response);

    std::string get_hostname() const { return hostname; }

    uint16_t get_port() const { return port; }

    std::unordered_set<std::pair<std::string, uint16_t>> get_replicas() const { return replicas; }

    void attach(std::string hostname, const uint16_t port);

    std::optional<grpc::ServerWriter<socketout::MessageResponse>*> get_message_stream(
        const std::string& username);

    std::optional<grpc::ServerWriter<socketout::ChannelResponse>*> get_channel_stream(
        const std::string& username);

    void remove_message_stream(const std::string& username);

    void remove_channel_stream(const std::string& username);

   private:
    std::string hostname;
    uint16_t port;
    std::unordered_map<std::string, grpc::ServerWriter<socketout::MessageResponse>*>
        message_streams;
    std::unordered_map<std::string, grpc::ServerWriter<socketout::ChannelResponse>*>
        channel_streams;
    std::unordered_set<std::pair<std::string, uint16_t>> replicas;
};
#endif
