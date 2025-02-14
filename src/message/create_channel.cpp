#include "message/create_channel.hpp"
#include <cstdint>
#include <vector>
#include "constants.hpp"
#include "json.hpp"
#include "message/header.hpp"

CreateChannelMessage::CreateChannelMessage(std::string channel_name, std::vector<UUID> members)
    : channel_name(channel_name), members(members) {}

void CreateChannelMessage::serialize(std::vector<uint8_t>& buf) const {
// Encode channel name length and channel name
#if PROTOCOL_JSON
    std::string msg = to_json();
    buf.insert(buf.end(), msg.begin(), msg.end());
#else
    uint8_t channel_name_length = this->channel_name.size();
    buf.push_back(channel_name_length);
    buf.insert(buf.end(), this->channel_name.begin(), this->channel_name.end());

    // Encode number of members
    uint8_t num_members = this->members.size();
    buf.push_back(num_members);

    // Encode each member
    for (const UUID& member : this->members) {
        member.serialize(buf);
    }
#endif
}

void CreateChannelMessage::serialize_msg(std::vector<uint8_t>& buf) const {
    Header header(PROTOCOL_VERSION, Operation::CREATE_CHANNEL, this->size());
    header.serialize(buf);

    this->serialize(buf);
}

void CreateChannelMessage::deserialize(const std::vector<uint8_t>& buf) {
#if PROTOCOL_JSON
    std::string msg(buf.begin(), buf.end());
    from_json(msg);
#else
    size_t offset = 0;

    uint8_t channel_name_length = buf[offset++];
    this->channel_name =
        std::string(buf.begin() + offset, buf.begin() + offset + channel_name_length);
    offset += channel_name_length;

    uint8_t num_members = buf[offset++];

    for (uint8_t i = 0; i < num_members; i++) {
        UUID member;
        member.deserialize(
            std::vector<uint8_t>(buf.begin() + offset, buf.begin() + offset + member.size()));
        this->members.push_back(member);
        offset += member.size();
    }
#endif
}

std::string CreateChannelMessage::to_json() const {
    nlohmann::json j;
    j["channel_name"] = this->channel_name;

    std::vector<std::string> member_strings;
    for (const UUID& member : this->members) {
        member_strings.push_back(member.to_string());
    }
    j["members"] = member_strings;

    return j.dump();
}

void CreateChannelMessage::from_json(const std::string& json) {
    nlohmann::json j = nlohmann::json::parse(json);
    this->channel_name = j["channel_name"].get<std::string>();
    for (const std::string& member_string : j["members"]) {
        this->members.push_back(UUID::from_string(member_string));
    }
}

size_t CreateChannelMessage::size() const {
#if PROTOCOL_JSON
    return to_json().size();
#else
    size_t size = 1 + this->channel_name.size() + 1;  // 1 for channel name length
    for (const UUID& member : this->members) {
        size += member.size();
    }
    return size;
#endif
}

const std::string& CreateChannelMessage::get_channel_name() const {
    return this->channel_name;
}

const std::vector<UUID>& CreateChannelMessage::get_members() const {
    return this->members;
}

void CreateChannelMessage::set_channel_name(std::string channel_name) {
    this->channel_name = channel_name;
}

void CreateChannelMessage::set_members(std::vector<UUID> members) {
    this->members = members;
}