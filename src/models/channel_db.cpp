#include "models/channel_db.hpp"

channel_db::channel_db() {}

const std::unordered_map<UUID, Channel> &channel_db::get_data() const {
    return this->data;
}

Channel channel_db::get_by_uid(UUID channel_uid) const {
    return this->data.at(channel_uid);
}

void channel_db::add_channel(Channel &channel) {
    this->data.insert({channel.get_uid(), channel});
}

void channel_db::remove_channel(UUID channel_uid) {
    this->data.erase(channel_uid);
}