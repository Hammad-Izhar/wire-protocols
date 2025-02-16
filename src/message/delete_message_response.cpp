#include "message/delete_message_response.hpp"
#include "constants.hpp"
#include "json.hpp"
#include "message/header.hpp"
#include "models/message.hpp"

DeleteMessageResponse::DeleteMessageResponse(std::variant<Message::SharedPtr, std::string> data)
    : data(data) {}

void DeleteMessageResponse::serialize(std::vector<uint8_t>& buf) const {
#if PROTOCOL_JSON
    std::string msg = to_json();
    buf.insert(buf.end(), msg.begin(), msg.end());
#else
    if (is_success()) {
        buf.push_back(0);
        std::get<Message::SharedPtr>(data)->serialize(buf);
    } else {
        buf.push_back(1);
        std::string error = std::get<std::string>(data);
        buf.push_back(error.size());
        buf.insert(buf.end(), error.begin(), error.end());
    }
#endif
}

void DeleteMessageResponse::serialize_msg(std::vector<uint8_t>& buf) const {
    Header header(PROTOCOL_VERSION, Operation::DELETE_MESSAGE, size());
    header.serialize(buf);
    serialize(buf);
}

void DeleteMessageResponse::deserialize(const std::vector<uint8_t>& buf) {
#if PROTOCOL_JSON
    std::string msg(buf.begin(), buf.end());
    from_json(msg);
#else
    if (buf[0] == 0) {
        Message::SharedPtr message = std::make_shared<Message>();
        message->deserialize(std::vector<uint8_t>(buf.begin() + 1, buf.end()));
        data = message;
    } else {
        size_t len = buf[1];
        std::string error(buf.begin() + 2, buf.begin() + 2 + len);
        data = error;
    }
#endif
}

std::string DeleteMessageResponse::to_json() const {
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

void DeleteMessageResponse::from_json(const std::string& json) {
    auto j = nlohmann::json::parse(json);
    if (j["success"].get<bool>()) {
        Message::SharedPtr message = std::make_shared<Message>();
        message->from_json(j["message"]);
        data = message;
    } else {
        data = j["error"].get<std::string>();
    }
}

size_t DeleteMessageResponse::size() const {
#if PROTOCOL_JSON
    return to_json().size();
#else
    if (std::holds_alternative<std::string>(data)) {
        return 1 + (1 + std::get<std::string>(data).size());
    }
    return 1 + std::get<Message::SharedPtr>(data)->size();
#endif
}

bool DeleteMessageResponse::is_success() const {
    return std::holds_alternative<Message::SharedPtr>(data);
}

std::optional<std::string> DeleteMessageResponse::get_error_message() const {
    if (std::holds_alternative<std::string>(data)) {
        return std::get<std::string>(data);
    }
    return std::nullopt;
}

std::optional<Message::SharedPtr> DeleteMessageResponse::get_data() const {
    if (std::holds_alternative<Message::SharedPtr>(data)) {
        return std::get<Message::SharedPtr>(data);
    }
    return std::nullopt;
}