#ifdef PROTOCOL_RPC
#include <grpcpp/server_context.h>
#include <grpcpp/support/status.h>
#include <optional>

#include "server/db/database.hpp"
#include "socketout_server.grpc.pb.h"
#include "socketout_server.pb.h"

class SocketOutServerImpl final : public socketout_server::SocketOutServer::Service {
    grpc::Status add_user(grpc::ServerContext* context,
                          const socketout_server::User* request,
                          google::protobuf::Empty* response) override {
        // Implementation for adding a user
        Database& db = Database::get_instance();
        std::optional<UUID> user_id = db.get_uid_from_username(request->username());

        if (user_id.has_value()) {
            std::cerr << "User already exists: " << request->username() << std::endl;
            return grpc::Status(grpc::StatusCode::ALREADY_EXISTS,
                                "User already exists in the replica");
        }

        User::SharedPtr user =
            std::make_shared<User>(request->username(), request->display_name(),
                                   UUID::from_string(request->uuid()), request->profile_picture());
        std::variant<std::monostate, std::string> result = db.add_user(user, request->password());

        if (std::holds_alternative<std::string>(result)) {
            return grpc::Status(grpc::StatusCode::INTERNAL, std::get<std::string>(result));
        }
        return grpc::Status::OK;
    }

    grpc::Status add_message(grpc::ServerContext* context,
                             const socketout_server::Message* request,
                             google::protobuf::Empty* response) override {
        Database& db = Database::get_instance();
        std::variant<Message::SharedPtr, std::string> result =
            db.add_message(UUID::from_string(request->sender_id()),
                           UUID::from_string(request->channel_id()), request->text());

        if (std::holds_alternative<std::string>(result)) {
            std::cerr << "Error adding message: " << std::get<std::string>(result) << std::endl;
            return grpc::Status(grpc::StatusCode::INTERNAL, std::get<std::string>(result));
        }

        return grpc::Status::OK;
    }

    grpc::Status add_channel(grpc::ServerContext* context,
                             const socketout_server::Channel* request,
                             google::protobuf::Empty* response) override {
        Database& db = Database::get_instance();
        std::vector<UUID> members;
        for (const auto& member : request->user_ids()) {
            members.push_back(UUID::from_string(member));
        }
        std::vector<uint64_t> message_snowflakes;
        for (const auto& snowflake : request->message_snowflakes()) {
            message_snowflakes.push_back(snowflake);
        }

        // Check if the channel already exists
        std::optional<Channel::SharedPtr> existing_channel =
            db.get_channel_by_uid(UUID::from_string(request->uuid()));
        if (existing_channel.has_value()) {
            std::cerr << "Channel already exists: " << request->channel_name() << std::endl;
            return grpc::Status(grpc::StatusCode::ALREADY_EXISTS,
                                "Channel already exists in the replica");
        }

        std::variant<Channel::SharedPtr, std::string> result =
            db.add_channel(request->channel_name(), members, UUID::from_string(request->uuid()),
                           message_snowflakes);

        if (std::holds_alternative<std::string>(result)) {
            std::cerr << "Error adding channel: " << std::get<std::string>(result) << std::endl;
            return grpc::Status(grpc::StatusCode::INTERNAL, std::get<std::string>(result));
        }

        return grpc::Status::OK;
    }

    grpc::Status delete_message(grpc::ServerContext* context,
                                const socketout_server::Snowflake* request,
                                google::protobuf::Empty* response) override {
        Database& db = Database::get_instance();
        std::variant<std::monostate, std::string> result = db.remove_message(request->snowflake());

        if (std::holds_alternative<std::string>(result)) {
            std::cerr << "Error deleting message: " << std::get<std::string>(result) << std::endl;
            return grpc::Status(grpc::StatusCode::INTERNAL, std::get<std::string>(result));
        }

        return grpc::Status::OK;
    }
    grpc::Status delete_user(grpc::ServerContext* context,
                             const socketout_server::UUID* request,
                             google::protobuf::Empty* response) override {
        Database& db = Database::get_instance();
        UUID user_uid = UUID::from_string(request->uuid());

        std::variant<User::SharedPtr, std::string> result = db.remove_user(user_uid);

        if (std::holds_alternative<std::string>(result)) {
            std::cerr << "Error deleting user: " << std::get<std::string>(result) << std::endl;
            return grpc::Status(grpc::StatusCode::INTERNAL, std::get<std::string>(result));
        }

        return grpc::Status::OK;
    }
};
#endif