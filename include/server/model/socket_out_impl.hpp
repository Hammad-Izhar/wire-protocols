#ifdef PROTOCOL_RPC
#include <grpcpp/server_context.h>
#include <grpcpp/support/status.h>
#include <grpcpp/support/sync_stream.h>
#include <iostream>
#include <variant>

#include "models/message.hpp"
#include "models/uuid.hpp"
#include "server/db/database.hpp"
#include "server/model/session.hpp"
#include "socketout.grpc.pb.h"
#include "socketout.pb.h"

class SocketOutImpl final : public socketout::SocketOut::Service {
    grpc::Status register_user(grpc::ServerContext* context,
                               const socketout::RegisterRequest* request,
                               google::protobuf::Empty* response) override {
        Database& db = Database::get_instance();
        std::optional<UUID> user_id = db.get_uid_from_username(request->username());

        if (user_id.has_value()) {
            return grpc::Status(grpc::StatusCode::ALREADY_EXISTS, "Username already exists");
        }

        User::SharedPtr user = std::make_shared<User>(request->username(), request->display_name());
        std::variant<std::monostate, std::string> result = db.add_user(user, request->password());
        if (std::holds_alternative<std::string>(result)) {
            return grpc::Status(grpc::StatusCode::INTERNAL, std::get<std::string>(result));
        }

        return grpc::Status::OK;
    };

    grpc::Status login_user(grpc::ServerContext* context,
                            const socketout::LoginRequest* request,
                            socketout::LoginResponse* response) override {
        Database& db = Database::get_instance();
        std::optional<UUID> user_uid = db.get_uid_from_username(request->username());
        if (!user_uid.has_value()) {
            return grpc::Status(grpc::StatusCode::NOT_FOUND, "Username does not exist");
        }
        std::variant<bool, std::string> result =
            db.verify_password(user_uid.value(), request->password());
        if (std::holds_alternative<std::string>(result)) {
            return grpc::Status(grpc::StatusCode::INTERNAL, std::get<std::string>(result));
        }
        if (!std::get<bool>(result)) {
            return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT,
                                "Username and password do not match");
        }

        // Print all messages using db's print_messages function
        db.print_messages();

        User::SharedPtr user = db.get_user_by_uid(user_uid.value()).value();
        socketout::User user_response;
        user_response.set_uuid(user->get_uid().to_string());
        user_response.set_username(user->get_username());
        user_response.set_display_name(user->get_display_name());
        user_response.set_profile_picture(user->get_profile_pic());
        response->mutable_user()->CopyFrom(user_response);

        return grpc::Status::OK;
    }
    grpc::Status subscribe_messages(
        grpc::ServerContext* context,
        const socketout::LoginRequest* request,
        grpc::ServerWriter<socketout::MessageResponse>* response) override {
        Database& db = Database::get_instance();
        Session& session = Session::get_instance();

        std::optional<UUID> user_uid = db.get_uid_from_username(request->username());
        if (!user_uid.has_value()) {
            return grpc::Status(grpc::StatusCode::NOT_FOUND, "Username does not exist");
        }
        std::variant<bool, std::string> result =
            db.verify_password(user_uid.value(), request->password());
        if (std::holds_alternative<std::string>(result)) {
            return grpc::Status(grpc::StatusCode::INTERNAL, std::get<std::string>(result));
        }
        if (!std::get<bool>(result)) {
            return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT,
                                "Username and password do not match");
        }

        session.save_message_stream(request->username(), response);

        User::SharedPtr user = db.get_user_by_uid(user_uid.value()).value();
        for (const auto& channel_uid : user->get_channels()) {
            std::optional<Channel::SharedPtr> channel = db.get_channel_by_uid(channel_uid);
            if (!channel.has_value()) {
                continue;
            }
            for (const auto& message_snowflake : channel.value()->get_message_snowflakes()) {
                std::optional<const Message::SharedPtr> message_opt =
                    db.get_message_by_uid(message_snowflake);
                if (!message_opt.has_value()) {
                    continue;
                }
                Message::SharedPtr message = message_opt.value();
                socketout::MessageResponse message_response;
                message_response.set_type(socketout::Operation::CREATE);
                socketout::Message* msg = message_response.mutable_msg();
                msg->set_sender_id(message->get_sender_id().to_string());
                msg->set_channel_id(message->get_channel_id().to_string());
                msg->set_snowflake(message->get_snowflake());
                msg->set_created_at(message->get_created_at());
                msg->set_modified_at(message->get_modified_at());
                msg->set_text(message->get_text());
                for (const auto& reader : message->get_read_by()) {
                    msg->add_read_by(reader.to_string());
                }
                response->Write(message_response);
            }
        }

        while (!context->IsCancelled()) {
            // wait for the user to disconnect
        }

        session.remove_message_stream(request->username());

        return grpc::Status::OK;
    };

    grpc::Status subscribe_channels(
        grpc::ServerContext* context,
        const socketout::LoginRequest* request,
        grpc::ServerWriter<socketout::ChannelResponse>* response) override {
        Database& db = Database::get_instance();
        Session& session = Session::get_instance();

        std::optional<UUID> user_uid = db.get_uid_from_username(request->username());
        if (!user_uid.has_value()) {
            return grpc::Status(grpc::StatusCode::NOT_FOUND, "Username does not exist");
        }
        std::variant<bool, std::string> result =
            db.verify_password(user_uid.value(), request->password());
        if (std::holds_alternative<std::string>(result)) {
            return grpc::Status(grpc::StatusCode::INTERNAL, std::get<std::string>(result));
        }
        if (!std::get<bool>(result)) {
            return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT,
                                "Username and password do not match");
        }

        session.save_channel_stream(request->username(), response);

        User::SharedPtr user = db.get_user_by_uid(user_uid.value()).value();
        for (const auto& channel_uid : user->get_channels()) {
            std::optional<Channel::SharedPtr> channel_opt = db.get_channel_by_uid(channel_uid);
            if (!channel_opt.has_value()) {
                continue;
            }
            Channel::SharedPtr channel = channel_opt.value();
            socketout::ChannelResponse channel_response;
            socketout::Channel* channel_msg = channel_response.mutable_channel();
            channel_response.set_type(socketout::Operation::CREATE);
            channel_msg->set_uuid(channel->get_uid().to_string());
            channel_msg->set_channel_name(channel->get_name());
            for (const auto& member : channel->get_user_uids()) {
                channel_msg->add_user_ids(member.to_string());
            }
            response->Write(channel_response);
        }

        while (!context->IsCancelled()) {
            // wait for the user to disconnect
        }

        session.remove_channel_stream(request->username());

        return grpc::Status::OK;
    };

    grpc::Status create_channel(grpc::ServerContext* context,
                                const socketout::CreateChannelRequest* request,
                                google::protobuf::Empty* response) override {
        std::vector<UUID> members;
        for (const auto& member : request->members()) {
            members.push_back(UUID::from_string(member));
        }

        Database& db = Database::get_instance();
        std::variant<Channel::SharedPtr, std::string> result =
            db.add_channel(request->channel_name(), members);

        if (std::holds_alternative<std::string>(result)) {
            return grpc::Status(grpc::StatusCode::INTERNAL, std::get<std::string>(result));
        }

        return grpc::Status::OK;
    };

    grpc::Status send_message(grpc::ServerContext* context,
                              const socketout::SendMessageRequest* request,
                              google::protobuf::Empty* response) override {
        Database& db = Database::get_instance();
        std::variant<Message::SharedPtr, std::string> result =
            db.add_message(UUID::from_string(request->sender_id()),
                           UUID::from_string(request->channel_id()), request->data());

        if (std::holds_alternative<std::string>(result)) {
            return grpc::Status(grpc::StatusCode::INTERNAL, std::get<std::string>(result));
        }

        return grpc::Status::OK;
    };

    grpc::Status delete_message(grpc::ServerContext* context,
                                const socketout::DeleteMessageRequest* request,
                                google::protobuf::Empty* response) override {
        Database& db = Database::get_instance();
        db.remove_message(request->message_id());

        return grpc::Status::OK;
    };

    grpc::Status delete_account(grpc::ServerContext* context,
                                const socketout::DeleteAccountRequest* request,
                                socketout::DeleteAccountResponse* response) override {
        Database& db = Database::get_instance();
        std::optional<UUID> user_uid = db.get_uid_from_username(request->username());
        if (!user_uid.has_value()) {
            return grpc::Status(grpc::StatusCode::NOT_FOUND, "Username does not exist");
        }

        std::variant<bool, std::string> result =
            db.verify_password(user_uid.value(), request->password());
        if (std::holds_alternative<std::string>(result)) {
            return grpc::Status(grpc::StatusCode::INTERNAL, std::get<std::string>(result));
        }
        if (!std::get<bool>(result)) {
            return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT,
                                "Username and password do not match");
        }

        std::variant<User::SharedPtr, std::string> delete_result = db.remove_user(user_uid.value());
        if (std::holds_alternative<std::string>(delete_result)) {
            return grpc::Status(grpc::StatusCode::INTERNAL, std::get<std::string>(delete_result));
        }

        User::SharedPtr user = std::get<User::SharedPtr>(delete_result);

        socketout::User user_response;
        user_response.set_uuid(user->get_uid().to_string());
        user_response.set_username(user->get_username());
        user_response.set_display_name(user->get_display_name());
        user_response.set_profile_picture(user->get_profile_pic());

        response->mutable_user()->CopyFrom(user_response);

        return grpc::Status::OK;
    };

    grpc::Status list_accounts(grpc::ServerContext* context,
                               const socketout::ListAccountsRequest* request,
                               socketout::ListAccountResponse* writer) override {
        Database& db = Database::get_instance();
        std::string regex_string = request->regex();

        std::variant<std::vector<UUID>, std::string> result =
            db.get_uuids_matching_regex(regex_string);
        if (std::holds_alternative<std::string>(result)) {
            return grpc::Status(grpc::StatusCode::INTERNAL, std::get<std::string>(result));
        }

        std::vector<UUID> uuids = std::get<std::vector<UUID>>(result);
        for (const auto& uuid : uuids) {
            std::optional<const User::SharedPtr> user = db.get_user_by_uid(uuid);
            if (user.has_value()) {
                socketout::User user_response;
                user_response.set_uuid(user.value()->get_uid().to_string());
                user_response.set_username(user.value()->get_username());
                user_response.set_display_name(user.value()->get_display_name());
                user_response.set_profile_picture(user.value()->get_profile_pic());
                writer->add_users()->CopyFrom(user_response);
            }
        }

        return grpc::Status::OK;
    };
};
#endif