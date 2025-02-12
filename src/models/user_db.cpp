// #include "models/user_db.hpp"

// user_db::user_db() {}

// const std::unordered_map<UUID, User> &user_db::get_data() const
// {
//     return this->data;
// }

// const User &user_db::get_by_uid(std::array<uint8_t,16> user_uid) const
// {
//     return this->data.at(user_uid);
// }

// void user_db::add_user(User &user)
// {
//     this->data.insert({user.get_uid(), user});
// }

// void user_db::remove_user(std::array<uint8_t,16> user_uid)
// {
//     this->data.erase(user_uid);
// }