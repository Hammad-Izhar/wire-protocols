#include <QTcpSocket>
#include <functional>
#include <iostream>
#include <typeindex>
#include <unordered_map>

#include "models/message_handlers.hpp"

class MessageHandler : public QObject {
    Q_OBJECT
   public:
    using HandlerFunction = std::function<void(void*, void*)>;
    MessageHandler() = default;

    static MessageHandler& get_instance() {
        thread_local MessageHandler instance;
        if (!instance.handlers_initialized) {
            init_message_handlers(instance);
        }

        return instance;
    }

    template <typename T>
    void register_handler(std::function<void(QTcpSocket*, T&)> handler) {
        handlers[typeid(T)] = [handler](void* socket, void* message) {
            handler(static_cast<QTcpSocket*>(socket), *static_cast<T*>(message));
        };
    }

    // Dispatch a message of a specific type
    template <typename T>
    void dispatch(QTcpSocket* socket, T& data) {
        qDebug() << "Trying to dispatch handler for: " << typeid(T).name();
        auto it = handlers.find(typeid(T));
        if (it != handlers.end()) {
            it->second(socket, &data);  // Call the stored function, passing data as void*
        } else {
            std::cerr << "No handler registered for type: " << typeid(T).name() << std::endl;
        }
    }

   signals:
    void write_data(std::vector<uint8_t> data);

   private:
    std::unordered_map<std::type_index, HandlerFunction> handlers;
    bool handlers_initialized = false;
};