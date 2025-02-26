#ifdef PROTOCOL_RPC
#include <grpcpp/support/sync_stream.h>
#include <optional>
#include <string>

#include "socketout.pb.h"

class Session {
   public:
    static Session& get_instance();

    void save_message_stream(const std::string& username,
                             grpc::ServerWriter<socketout::MessageResponse>* response);

    void save_channel_stream(const std::string& username,
                             grpc::ServerWriter<socketout::ChannelResponse>* response);

    std::optional<grpc::ServerWriter<socketout::MessageResponse>*> get_message_stream(
        const std::string& username);

    std::optional<grpc::ServerWriter<socketout::ChannelResponse>*> get_channel_stream(
        const std::string& username);

    void remove_message_stream(const std::string& username);

    void remove_channel_stream(const std::string& username);

   private:
    std::unordered_map<std::string, grpc::ServerWriter<socketout::MessageResponse>*>
        message_streams;
    std::unordered_map<std::string, grpc::ServerWriter<socketout::ChannelResponse>*>
        channel_streams;
};
#endif
