#pragma once
#include <unistd.h>
#include "models/uuid.hpp"
#ifdef PROTOCOL_RPC
#include <google/protobuf/empty.pb.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/server_context.h>
#include <grpcpp/support/status.h>
#include <optional>

#include "server/db/database.hpp"
#include "server/model/session.hpp"
#include "socketout_server.grpc.pb.h"
#include "socketout_server.pb.h"

class SocketOutServerImpl final : public socketout_server::SocketOutServer::Service {
    grpc::Status attach(grpc::ServerContext* context,
                        const socketout_server::AttachRequest* request,
                        socketout_server::AttachRequest* response) override {
        std::cout << "Attempting to attach to replica: " << request->hostname() << request->port()
                  << std::endl;

        Session& session = Session::get_instance();
        session.attach(request->hostname(), request->port());

        // loop over all the users, channels, and databses and make the corresponding calls to the
        // remote:
        auto channel =
            grpc::CreateChannel(request->hostname() + ":" + std::to_string(request->port()),
                                grpc::InsecureChannelCredentials());
        auto stub = socketout_server::SocketOutServer::NewStub(channel);

        Database& db = Database::get_instance();
        auto users = db.get_all_users();
        auto channels = db.get_all_channels();
        auto messages = db.get_all_messages();

        for (const auto& user : users) {
            socketout_server::UserPassSalt user_request;
            user_request.set_username(user->get_username());
            user_request.set_display_name(user->get_display_name());
            user_request.set_uuid(user->get_uid().to_string());
            user_request.set_profile_picture(user->get_profile_pic());
            auto [pass, salt] = std::get<std::pair<std::string, std::string>>(
                db.get_password_and_salt(user->get_uid()));
            user_request.set_hashed_password(pass);
            user_request.set_salt(salt);

            grpc::ClientContext add_user_context;
            google::protobuf::Empty add_user_response;
            grpc::Status status = stub->add_user_with_hashed_password_and_salt(
                &add_user_context, user_request, &add_user_response);
            if (!status.ok()) {
                std::cerr << "Failed to add user: " << status.error_message() << std::endl;
            }
        }

        for (const auto& channel : channels) {
            socketout_server::Channel channel_request;
            channel_request.set_channel_name(channel->get_name());
            channel_request.set_uuid(channel->get_uid().to_string());
            for (const auto& member : channel->get_user_uids()) {
                channel_request.add_user_ids(member.to_string());
            }
            for (const auto& snowflake : channel->get_message_snowflakes()) {
                channel_request.add_message_snowflakes(snowflake);
            }
            grpc::ClientContext add_channel_context;
            google::protobuf::Empty add_channel_response;
            grpc::Status status =
                stub->add_channel(&add_channel_context, channel_request, &add_channel_response);
            if (!status.ok()) {
                std::cerr << "Failed to add channel: " << status.error_message() << std::endl;
            }
        }

        for (const auto& message : messages) {
            socketout_server::Message message_request;
            message_request.set_sender_id(message->get_sender_id().to_string());
            message_request.set_channel_id(message->get_channel_id().to_string());
            message_request.set_text(message->get_text());
            message_request.set_snowflake(message->get_snowflake());
            message_request.set_created_at(message->get_created_at());
            message_request.set_modified_at(message->get_modified_at());

            for (const auto& reader : message->get_read_by()) {
                message_request.add_read_by(reader.to_string());
            }

            grpc::ClientContext add_message_context;
            google::protobuf::Empty add_message_response;
            grpc::Status status =
                stub->add_message(&add_message_context, message_request, &add_message_response);
            if (!status.ok()) {
                std::cerr << "Failed to add message: " << status.error_message() << std::endl;
            }
        }

        response->set_hostname(session.get_hostname());
        response->set_port(session.get_port());
        return grpc::Status::OK;
    }

    grpc::Status add_user_with_hashed_password_and_salt(
        grpc::ServerContext* context,
        const socketout_server::UserPassSalt* request,
        google::protobuf::Empty* response) override {
        std::cout << "Attempting to add user copy received from another replica: "
                  << request->uuid() << std::endl;

        // Implementation for adding a user
        Database& db = Database::get_instance();
        std::optional<UUID> user_id = db.get_uid_from_username(request->username());

        if (user_id.has_value()) {
            std::cerr << "Failed to add user copy since user already exists: "
                      << request->username() << std::endl;
            return grpc::Status(grpc::StatusCode::ALREADY_EXISTS,
                                "User already exists in the replica");
        }

        User::SharedPtr user =
            std::make_shared<User>(request->username(), request->display_name(),
                                   UUID::from_string(request->uuid()), request->profile_picture());
        std::variant<std::monostate, std::string> result =
            db.add_user_with_hashed_password_and_salt(user, request->hashed_password(),
                                                      request->salt());

        if (std::holds_alternative<std::string>(result)) {
            std::cerr << "Error adding user: " << std::get<std::string>(result) << std::endl;
            return grpc::Status(grpc::StatusCode::INTERNAL, std::get<std::string>(result));
        }

        return grpc::Status::OK;
    }

    grpc::Status add_user(grpc::ServerContext* context,
                          const socketout_server::User* request,
                          google::protobuf::Empty* response) override {
        std::cout << "Attempting to add user copy received from another replica: "
                  << request->uuid() << std::endl;

        // Implementation for adding a user
        Database& db = Database::get_instance();
        std::optional<UUID> user_id = db.get_uid_from_username(request->username());

        if (user_id.has_value()) {
            std::cerr << "Failed to add user copy since user already exists: "
                      << request->username() << std::endl;
            return grpc::Status(grpc::StatusCode::ALREADY_EXISTS,
                                "User already exists in the replica");
        }

        User::SharedPtr user =
            std::make_shared<User>(request->username(), request->display_name(),
                                   UUID::from_string(request->uuid()), request->profile_picture());
        std::variant<std::monostate, std::string> result = db.add_user(user, request->password());

        if (std::holds_alternative<std::string>(result)) {
            std::cerr << "Error adding user: " << std::get<std::string>(result) << std::endl;
            return grpc::Status(grpc::StatusCode::INTERNAL, std::get<std::string>(result));
        }

        return grpc::Status::OK;
    }

    grpc::Status add_message(grpc::ServerContext* context,
                             const socketout_server::Message* request,
                             google::protobuf::Empty* response) override {
        std::cout << "Attempting to add message copy received from another replica: "
                  << request->text() << std::endl;

        std::vector<UUID> read_by;
        for (const auto& reader : request->read_by()) {
            read_by.push_back(UUID::from_string(reader));
        }

        Database& db = Database::get_instance();
        std::variant<Message::SharedPtr, std::string> result = db.add_message(
            UUID::from_string(request->sender_id()), UUID::from_string(request->channel_id()),
            request->text(), request->snowflake(), request->created_at(), request->modified_at(),
            read_by);

        if (std::holds_alternative<std::string>(result)) {
            std::cerr << "Failed to add message copy:  " << std::get<std::string>(result)
                      << std::endl;
            return grpc::Status(grpc::StatusCode::INTERNAL, std::get<std::string>(result));
        }

        return grpc::Status::OK;
    }

    grpc::Status add_channel(grpc::ServerContext* context,
                             const socketout_server::Channel* request,
                             google::protobuf::Empty* response) override {
        std::cout << "Attempting to add channel copy received from another replica: "
                  << request->uuid() << std::endl;

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
            std::cerr << "!Channel already exists: " << request->channel_name() << std::endl;
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
        std::cout << "Attempting to delete message copy received from another replica: "
                  << request->snowflake() << std::endl;

        Database& db = Database::get_instance();
        std::variant<std::monostate, std::string> result = db.remove_message(request->snowflake());

        if (std::holds_alternative<std::string>(result)) {
            std::cerr << "!Error deleting message: " << std::get<std::string>(result) << std::endl;
            return grpc::Status(grpc::StatusCode::INTERNAL, std::get<std::string>(result));
        }

        return grpc::Status::OK;
    }

    grpc::Status delete_user(grpc::ServerContext* context,
                             const socketout_server::UUID* request,
                             google::protobuf::Empty* response) override {
        std::cout << "Attempting to delete user copy received from another replica: "
                  << request->uuid() << std::endl;

        Database& db = Database::get_instance();
        UUID user_uid = UUID::from_string(request->uuid());

        std::variant<User::SharedPtr, std::string> result = db.remove_user(user_uid);

        if (std::holds_alternative<std::string>(result)) {
            std::cerr << "!Error deleting user: " << std::get<std::string>(result) << std::endl;
            return grpc::Status(grpc::StatusCode::INTERNAL, std::get<std::string>(result));
        }

        return grpc::Status::OK;
    }
};
#endif