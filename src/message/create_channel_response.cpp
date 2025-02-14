#include "message/create_channel_response.hpp"
#include <memory>
#include <vector>
#include "constants.hpp"
#include "json.hpp"
#include "message/header.hpp"
#include "models/channel.hpp"

CreateChannelResponse::CreateChannelResponse(std::variant<Channel::SharedPtr, std::string> data)
    : data(std::move(data)) {}

void CreateChannelResponse::serialize(std::vector<uint8_t>& buf) const {
    if (std::holds_alternative<Channel::SharedPtr>(data)) {
        buf.push_back(0);
        std::get<Channel::SharedPtr>(data)->serialize(buf);
    } else {
        buf.push_back(1);
        const std::string& error = std::get<std::string>(data);
        buf.push_back(error.size());
        buf.insert(buf.end(), error.begin(), error.end());
    }
}

void CreateChannelResponse::serialize_msg(std::vector<uint8_t>& buf) const {
    Header header(PROTOCOL_VERSION, Operation::CREATE_CHANNEL, this->size());
    header.serialize(buf);
    serialize(buf);
}

void CreateChannelResponse::deserialize(const std::vector<uint8_t>& buf) {
    size_t offset = 0;
    uint8_t has_error = buf[offset++];
    if (has_error == 0) {
        Channel::SharedPtr channel = std::make_shared<Channel>();
        channel->deserialize(std::vector<uint8_t>(buf.begin() + offset, buf.end()));
        data = channel;
    } else {
        uint8_t error_length = buf[offset++];
        std::string error(buf.begin() + offset, buf.begin() + offset + error_length);
        data = error;
    }
}

std::string CreateChannelResponse::to_json() const {
    nlohmann::json j;
    if (std::holds_alternative<Channel::SharedPtr>(data)) {
        j["channel"] = std::get<Channel::SharedPtr>(data)->to_json();
    } else {
        j["error"] = std::get<std::string>(data);
    }
    return j.dump();
}

void CreateChannelResponse::from_json(const std::string& json) {
    nlohmann::json j = nlohmann::json::parse(json);
    if (j.contains("channel")) {
        Channel::SharedPtr channel = std::make_shared<Channel>();
        channel->from_json(j["channel"].dump());
        data = channel;
    } else {
        data = j["error"].get<std::string>();
    }
}

[[nodiscard]] size_t CreateChannelResponse::size() const {
    size_t size = 1;  // for the has_error byte
    if (std::holds_alternative<Channel::SharedPtr>(data)) {
        size += std::get<Channel::SharedPtr>(data)->size();
    } else {
        const std::string& error = std::get<std::string>(data);
        size += 1 + error.size();  // 1 for the error length + error length
    }
    return size;
}

[[nodiscard]] bool CreateChannelResponse::is_success() const {
    return std::holds_alternative<Channel::SharedPtr>(data);
}

[[nodiscard]] std::optional<std::string> CreateChannelResponse::get_error_message() const {
    if (is_success()) {
        return std::nullopt;
    } else {
        return std::get<std::string>(data);
    }
}

[[nodiscard]] std::optional<Channel::SharedPtr> CreateChannelResponse::get_data() {
    if (is_success()) {
        return std::get<Channel::SharedPtr>(data);
    } else {
        return std::nullopt;
    }
}