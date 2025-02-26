#include <QDebug>

#include <qtmetamacros.h>
#include "models/message.hpp"
#include "server/db/database.hpp"
#include "server/model/session.hpp"
#ifdef PROTOCOL_RPC
#include "socketout.pb.h"
#endif

Database::Database() {
    this->users = std::make_unique<UserTable>();
    this->messages = std::make_unique<MessageTable>();
    this->channels = std::make_unique<ChannelTable>();
    this->passwords = std::make_unique<PasswordTable>();
}

Database& Database::get_instance() {
    static Database instance;
    return instance;
}

const std::optional<const User::SharedPtr> Database::get_user_by_uid(UUID user_uid) const {
    return this->users->get_by_uid(user_uid);
}

const std::optional<const Message::SharedPtr> Database::get_message_by_uid(
    uint64_t message_snowflake) const {
    return this->messages->get_by_uid(message_snowflake);
}

const std::optional<const Channel::SharedPtr> Database::get_channel_by_uid(UUID channel_uid) const {
    return this->channels->get_by_uid(channel_uid);
}

std::optional<User::SharedPtr> Database::get_mut_user_by_uid(UUID user_uid) {
    return this->users->get_mut_by_uid(user_uid);
}

std::optional<Message::SharedPtr> Database::get_mut_message_by_uid(uint64_t message_snowflake) {
    return this->messages->get_mut_by_uid(message_snowflake);
}

std::optional<Channel::SharedPtr> Database::get_mut_channel_by_uid(UUID channel_uid) {
    return this->channels->get_mut_by_uid(channel_uid);
}

std::variant<std::vector<UUID>, std::string> Database::get_uuids_matching_regex(
    std::string regex) const {
    return this->users->get_uuids_matching_regex(regex);
}

std::optional<UUID> Database::get_uid_from_username(std::string username) {
    return this->users->get_uid_from_username(username);
}

std::variant<bool, std::string> Database::verify_password(UUID& user_uid, std::string password) {
    return this->passwords->verify_password(user_uid, password);
}

std::variant<std::monostate, std::string> Database::add_user(User::SharedPtr user,
                                                             std::string password) {
    // Add the password
    UUID user_uid = user->get_uid();
    std::variant<std::monostate, std::string> res =
        this->passwords->add_password(user_uid, password);
    if (std::holds_alternative<std::string>(res)) {
        return std::get<std::string>(res);
    }
    // Add the user
    return this->users->add_user(user);
}

std::variant<Message::SharedPtr, std::string> Database::add_message(UUID sender_uid,
                                                                    UUID channel_uid,
                                                                    std::string content) {
    std::optional<Channel::SharedPtr> channel = this->channels->get_mut_by_uid(channel_uid);
    if (!channel.has_value()) {
        return "Channel does not exist";
    }

    auto res = this->messages->add_message(sender_uid, channel_uid, content);
    if (std::holds_alternative<std::string>(res)) {
        return std::get<std::string>(res);
    }
    Message::SharedPtr message = std::get<Message::SharedPtr>(res);

    channel.value()->add_message(message->get_snowflake());

#ifdef PROTOCOL_RPC
    socketout::Message msg;
    msg.set_sender_id(message->get_sender_id().to_string());
    msg.set_channel_id(message->get_channel_id().to_string());
    msg.set_snowflake(message->get_snowflake());
    msg.set_created_at(message->get_created_at());
    msg.set_modified_at(message->get_modified_at());
    msg.set_text(message->get_text());
#endif

    for (auto user_uid : channel.value()->get_user_uids()) {
        std::optional<User::SharedPtr> user = this->users->get_mut_by_uid(user_uid);
        if (!user.has_value()) {
            continue;
        }
#ifdef PROTOCOL_RPC
        Session& session = Session::get_instance();
        std::optional<grpc::ServerWriter<socketout::MessageResponse>*> writer =
            session.get_message_stream(user.value()->get_username());

        for (auto& user_uid : message->get_read_by()) {
            msg.add_read_by(user_uid.to_string());
        }

        if (writer.has_value()) {
            socketout::MessageResponse response;
            response.set_type(socketout::Operation::CREATE);
            response.mutable_msg()->CopyFrom(msg);
            writer.value()->Write(response);
        }
#else
        emit user.value()->message_received(message);
#endif
    }

    return message;
}

std::variant<Channel::SharedPtr, std::string> Database::add_channel(std::string channel_name,
                                                                    std::vector<UUID> members) {
    auto res = this->channels->add_channel(channel_name, members);
    if (std::holds_alternative<std::string>(res)) {
        return std::get<std::string>(res);
    }
    Channel::SharedPtr channel = std::get<Channel::SharedPtr>(res);

#if PROTOCOL_RPC
    socketout::Channel chnl;
    chnl.set_channel_name(channel->get_name());
    chnl.set_uuid(channel->get_uid().to_string());
    for (int i = 0; i < channel->get_user_uids().size(); i++) {
        chnl.set_user_ids(i, channel->get_user_uids()[i].to_string());
    }
    for (int i = 0; i < channel->get_message_snowflakes().size(); i++) {
        chnl.set_message_snowflakes(i, channel->get_message_snowflakes()[i]);
    }
#endif

    for (auto& user_uid : channel->get_user_uids()) {
        std::optional<User::SharedPtr> user = this->users->get_mut_by_uid(user_uid);
        if (!user.has_value()) {
            continue;
        }
#ifdef PROTOCOL_RPC
        Session& session = Session::get_instance();
        std::optional<grpc::ServerWriter<socketout::ChannelResponse>*> writer =
            session.get_channel_stream(user.value()->get_username());
        if (writer.has_value()) {
            socketout::ChannelResponse response;
            response.set_type(socketout::Operation::CREATE);
            response.mutable_channel()->CopyFrom(chnl);
            writer.value()->Write(response);
        }
#else
        user.value()->add_channel(channel->get_uid());
        emit user.value()->channel_added(channel);
#endif
    }

    return channel;
}

std::variant<std::monostate, std::string> Database::add_user_to_channel(UUID user_uid,
                                                                        UUID channel_uid) {
    std::optional<User::SharedPtr> user = this->users->get_mut_by_uid(user_uid);
    if (!user.has_value()) {
        return "User does not exist";
    }

    std::optional<Channel::SharedPtr> channel = this->channels->get_mut_by_uid(channel_uid);
    if (!channel.has_value()) {
        return "Channel does not exist";
    }

    user.value()->add_channel(channel_uid);
    channel.value()->add_user(user_uid);
    return {};
}

std::variant<User::SharedPtr, std::string> Database::remove_user(UUID user_uid) {
    std::optional<User::SharedPtr> user = this->users->get_mut_by_uid(user_uid);
    if (!user.has_value()) {
        return "User does not exist";
    }

    for (auto& channel_uid : user.value()->get_channels()) {
        std::optional<Channel::SharedPtr> channel_opt = this->channels->get_mut_by_uid(channel_uid);
        if (!channel_opt.has_value()) {
            continue;
        }
        Channel::SharedPtr channel = channel_opt.value();
        channel->remove_user(user_uid);

        // For each message in the channel, remove those messages from the user
        for (auto& message_snowflake : channel->get_message_snowflakes()) {
            std::optional<const Message::SharedPtr> message_opt =
                this->messages->get_by_uid(message_snowflake);
            if (!message_opt.has_value()) {
                continue;
            }

            if (message_opt.value()->get_sender_id() == user_uid) {
                channel->remove_message(message_snowflake);
#ifdef PROTOCOL_RPC
                socketout::Message msg;
                msg.set_sender_id(message_opt.value()->get_sender_id().to_string());
                msg.set_channel_id(message_opt.value()->get_channel_id().to_string());
                msg.set_snowflake(message_opt.value()->get_snowflake());
                msg.set_created_at(message_opt.value()->get_created_at());
                msg.set_modified_at(message_opt.value()->get_modified_at());
                msg.set_text(message_opt.value()->get_text());
#endif
                for (auto& user_uid : channel->get_user_uids()) {
                    std::optional<User::SharedPtr> user = this->users->get_mut_by_uid(user_uid);
                    if (!user.has_value()) {
                        continue;
                    }
#ifdef PROTOCOL_RPC
                    Session& session = Session::get_instance();
                    std::optional<grpc::ServerWriter<socketout::MessageResponse>*> writer =
                        session.get_message_stream(user.value()->get_username());
                    socketout::MessageResponse response;
                    response.set_type(socketout::Operation::DELETE);
                    response.mutable_msg()->CopyFrom(msg);
                    writer.value()->Write(response);
#else
                    emit user.value()->message_deleted(message_opt.value());
#endif
                }
            }
        }
    }

    return this->users->remove_user(user_uid);
}

std::variant<std::monostate, std::string> Database::remove_message(uint64_t message_snowflake) {
    std::optional<const Message::SharedPtr> message = this->messages->get_by_uid(message_snowflake);
    if (!message.has_value()) {
        return "Message does not exist";
    }

    std::optional<Channel::SharedPtr> channel =
        this->channels->get_mut_by_uid(message.value()->get_channel_id());
    if (!channel.has_value()) {
        return "Channel does not exist";
    }

    auto res = this->messages->remove_message(message_snowflake);
    if (std::holds_alternative<std::string>(res)) {
        return std::get<std::string>(res);
    }

#ifdef PROTOCOL_RPC
    socketout::Message msg;
    msg.set_sender_id(message.value()->get_sender_id().to_string());
    msg.set_channel_id(message.value()->get_channel_id().to_string());
    msg.set_snowflake(message.value()->get_snowflake());
    msg.set_created_at(message.value()->get_created_at());
    msg.set_modified_at(message.value()->get_modified_at());
    msg.set_text(message.value()->get_text());
#endif
    for (auto user_uid : channel.value()->get_user_uids()) {
        std::optional<User::SharedPtr> user = this->users->get_mut_by_uid(user_uid);
        if (!user.has_value()) {
            continue;
        }

#ifdef PROTOCOL_RPC
        Session& session = Session::get_instance();
        std::optional<grpc::ServerWriter<socketout::MessageResponse>*> writer =
            session.get_message_stream(user.value()->get_username());
        socketout::MessageResponse response;
        response.set_type(socketout::Operation::DELETE);
        response.mutable_msg()->CopyFrom(msg);
        writer.value()->Write(response);
#else
        emit user.value()->message_deleted(message.value());
#endif
    }

    channel.value()->remove_message(message_snowflake);
    return {};
}

std::variant<std::monostate, std::string> Database::remove_channel(UUID channel_uid) {
    std::optional<const Channel::SharedPtr> channel = this->channels->get_by_uid(channel_uid);

    for (auto& user_uid : channel.value()->get_user_uids()) {
        std::optional<User::SharedPtr> user = this->users->get_mut_by_uid(user_uid);
        if (!user.has_value()) {
            continue;
        }
        user.value()->remove_channel(channel_uid);
    }

    for (auto& message_snowflake : channel.value()->get_message_snowflakes()) {
        std::optional<Message::SharedPtr> message =
            this->messages->get_mut_by_uid(message_snowflake);
        if (!message.has_value()) {
            continue;
        }
        this->messages->remove_message(message_snowflake);
    }

    return this->channels->remove_channel(channel_uid);
}
