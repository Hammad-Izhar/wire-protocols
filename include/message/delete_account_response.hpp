#include <optional>
#include <string>
#include <variant>

#include "message/serialize.hpp"

class DeleteAccountResponse : public Serializable {
   public:
    DeleteAccountResponse() = default;

    DeleteAccountResponse(std::variant<std::monostate, std::string> data);

    void serialize(std::vector<uint8_t>& buf) const;

    void serialize_msg(std::vector<uint8_t>& buf) const;

    void deserialize(const std::vector<uint8_t>& buf);

    [[nodiscard]] size_t size() const;

    [[nodiscard]] bool is_success() const;

    [[nodiscard]] std::optional<std::string> get_error_message() const;

   private:
    std::variant<std::monostate, std::string> data;
};