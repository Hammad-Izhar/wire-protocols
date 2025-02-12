#include <variant>
#include <functional>
#include <unordered_map>
#include <typeindex>

class MessageHandler
{
public:
    using HandlerFunction = std::function<void(void *)>;
    MessageHandler() = default;

    static MessageHandler &get_instance()
    {
        static MessageHandler instance;
        return instance;
    }

    template <typename T>
    void register_handler(std::function<void(T &)> handler)
    {
        handlers[typeid(T)] = [handler](void *message)
        {
            handler(*static_cast<T *>(message));
        };
    }

    // Dispatch a message of a specific type
    template <typename T>
    void dispatch(T &data)
    {
        auto it = handlers.find(typeid(T));
        if (it != handlers.end())
        {
            it->second(&data); // Call the stored function, passing data as void*
        }
        else
        {
            std::cerr << "No handler registered for type: " << typeid(T).name() << std::endl;
        }
    }

private:
    std::unordered_map<std::type_index, HandlerFunction> handlers;
};