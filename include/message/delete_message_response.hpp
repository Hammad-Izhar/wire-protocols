#include <optional>
#include <string>
#include <variant>

#include "message/serialize.hpp"
#include "models/message.hpp"

class DeleteMessageResponse : public Serializable {
   public:
    DeleteMessageResponse() = default;

    DeleteMessageResponse(std::variant<Message::SharedPtr, std::string> data);

    void serialize(std::vector<uint8_t>& buf) const;

    void serialize_msg(std::vector<uint8_t>& buf) const;

    void deserialize(const std::vector<uint8_t>& buf);

    [[nodiscard]] std::string to_json() const;

    void from_json(const std::string& json);

    [[nodiscard]] size_t size() const;

    [[nodiscard]] bool is_success() const;

    [[nodiscard]] std::optional<std::string> get_error_message() const;

    [[nodiscard]] std::optional<Message::SharedPtr> get_data() const;

   private:
    std::variant<Message::SharedPtr, std::string> data;
};