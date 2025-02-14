#include "message/send_message_response.hpp"
#include "constants.hpp"
#include "json.hpp"
#include "message/header.hpp"

SendMessageResponse::SendMessageResponse(std::variant<Message::SharedPtr, std::string> data)
    : data(std::move(data)) {}

void SendMessageResponse::serialize(std::vector<uint8_t>& buf) const {
#if PROTOCOL_JSON
    std::string msg = to_json();
    buf.insert(buf.end(), msg.begin(), msg.end());
#else
    if (std::holds_alternative<Message::SharedPtr>(data)) {
        buf.push_back(0);
        std::get<Message::SharedPtr>(data)->serialize(buf);
    } else {
        buf.push_back(1);
        const std::string& error = std::get<std::string>(data);
        buf.push_back(error.size());
        buf.insert(buf.end(), error.begin(), error.end());
    }
#endif
}

void SendMessageResponse::serialize_msg(std::vector<uint8_t>& buf) const {
    Header header(PROTOCOL_VERSION, Operation::SEND_MESSAGE, this->size());
    header.serialize(buf);
    serialize(buf);
}

void SendMessageResponse::deserialize(const std::vector<uint8_t>& buf) {
#if PROTOCOL_JSON
    from_json(std::string(buf.begin(), buf.end()));
#else
    size_t offset = 0;
    uint8_t has_error = buf[offset++];
    if (has_error == 0) {
        data = std::make_shared<Message>();
        std::get<Message::SharedPtr>(data)->deserialize(
            std::vector<uint8_t>(buf.begin() + offset, buf.end()));
    } else {
        uint8_t error_length = buf[offset++];
        std::string error(buf.begin() + offset, buf.begin() + offset + error_length);
        data = error;
    }
#endif
}

std::string SendMessageResponse::to_json() const {
    nlohmann::json j;
    if (is_success()) {
        j["success"] = true;
        j["message"] = std::get<Message::SharedPtr>(data)->to_json();
    } else {
        j["success"] = false;
        j["error"] = std::get<std::string>(data);
    }
    return j.dump();
}

void SendMessageResponse::from_json(const std::string& json) {
    auto j = nlohmann::json::parse(json);
    if (j["success"].get<bool>()) {
        Message::SharedPtr message = std::make_shared<Message>();
        message->from_json(j["message"]);
        data = message;
    } else {
        data = j["error"].get<std::string>();
    }
}

[[nodiscard]] size_t SendMessageResponse::size() const {
#if PROTOCOL_JSON
    return to_json().size();
#else
    size_t size = 1;  // for the has_error byte
    if (std::holds_alternative<std::string>(data)) {
        const std::string& error = std::get<std::string>(data);
        size += 1 + error.size();  // 1 for the error length + error length
    } else {
        size += std::get<Message::SharedPtr>(data)->size();
    }
    return size;
#endif
}

[[nodiscard]] bool SendMessageResponse::is_success() const {
    return std::holds_alternative<Message::SharedPtr>(data);
}

[[nodiscard]] std::optional<std::string> SendMessageResponse::get_error_message() const {
    if (is_success()) {
        return std::nullopt;
    } else {
        return std::get<std::string>(data);
    }
}

[[nodiscard]] std::optional<Message::SharedPtr> SendMessageResponse::get_data() const {
    if (is_success()) {
        return std::get<Message::SharedPtr>(data);
    }
    return std::nullopt;
}