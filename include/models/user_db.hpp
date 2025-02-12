// #include <unordered_map>
// #include "models/user.hpp"

// class user_db
// {
// public:
//     user_db() = default;

//     // Getters
//     [[nodiscard]] const std::unordered_map<std::array<uint8_t,16>, User> &get_data() const;
//     [[nodiscard]] const User &get_by_uid(std::array<uint8_t,16> user_uid) const;

//     // Setters
//     void add_user(User &user);
//     void remove_user(std::array<uint8_t,16> user_uid);
// private:
//     std::unordered_map<std::array<uint8_t,16>, User> data;
// };