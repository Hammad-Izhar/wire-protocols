#include <variant>
#include <functional>
#include <unordered_map>
#include <typeindex>
#include <QTcpSocket>

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
    void register_handler(std::function<void(QTcpSocket *, T &)> handler)
    {
        handlers[typeid(T)] = [handler](void *socket, void *message)
        {
            handler(static_cast<QTcpSocket *> socket, *static_cast<T *>(message));
        };
    }

    // Dispatch a message of a specific type
    template <typename T>
    void dispatch(QTcpSocket *socket, T &data)
    {
        auto it = handlers.find(typeid(T));
        if (it != handlers.end())
        {
            it->second(socket, &data); // Call the stored function, passing data as void*
        }
        else
        {
            std::cerr << "No handler registered for type: " << typeid(T).name() << std::endl;
        }
    }

private:
    std::unordered_map<std::type_index, HandlerFunction> handlers;
};