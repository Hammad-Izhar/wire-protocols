#include <optional>
#include <string>
#include <variant>

#include "message/serialize.hpp"
#include "models/channel.hpp"

class CreateChannelResponse : public Serializable {
   public:
    CreateChannelResponse() = default;

    CreateChannelResponse(std::variant<Channel::SharedPtr, std::string> data);

    void serialize(std::vector<uint8_t>& buf) const;

    void serialize_msg(std::vector<uint8_t>& buf) const;

    void deserialize(const std::vector<uint8_t>& buf);

    [[nodiscard]] std::string to_json() const;

    void from_json(const std::string& json);

    [[nodiscard]] size_t size() const;

    [[nodiscard]] bool is_success() const;

    [[nodiscard]] std::optional<std::string> get_error_message() const;

    [[nodiscard]] std::optional<Channel::SharedPtr> get_data();

   private:
    std::variant<Channel::SharedPtr, std::string> data;
};