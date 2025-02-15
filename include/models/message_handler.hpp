#include <QTcpSocket>
#include <functional>
#include <iostream>
#include <typeindex>
#include <unordered_map>

#include "models/message_handlers.hpp"

/**
 * @brief Handles registration and dispatching of message handlers.
 *
 * The MessageHandler class is responsible for registering handler functions for various message types
 * and dispatching messages to the appropriate handler based on their type. It follows a singleton pattern,
 * providing a thread-local instance. Additionally, it emits a Qt signal to write data.
 */
class MessageHandler : public QObject {
    Q_OBJECT
   public:
    /**
     * @brief Alias for the handler function type.
     *
     * The HandlerFunction is a std::function that accepts two void pointers:
     * one for a QTcpSocket pointer and one for a message pointer.
     */
    using HandlerFunction = std::function<void(void*, void*)>;

    /**
     * @brief Default constructor.
     *
     * Initializes a new instance of the MessageHandler class.
     */
    MessageHandler() = default;

    /**
     * @brief Retrieves the singleton instance of the MessageHandler.
     *
     * Returns a thread-local instance of MessageHandler. If the handlers have not yet been initialized,
     * they are initialized via the external function init_message_handlers.
     *
     * @return Reference to the singleton MessageHandler instance.
     */
    static MessageHandler& get_instance() {
        thread_local MessageHandler instance;
        if (!instance.handlers_initialized) {
            init_message_handlers(instance);
        }
        return instance;
    }

    /**
     * @brief Registers a handler for a specific message type.
     *
     * Associates a handler function with the message type T. The provided handler should accept
     * a QTcpSocket pointer and a reference to a message of type T.
     *
     * @tparam T The type of the message to be handled.
     * @param handler The function to be invoked when a message of type T is dispatched.
     */
    template <typename T>
    void register_handler(std::function<void(QTcpSocket*, T&)> handler) {
        handlers[typeid(T)] = [handler](void* socket, void* message) {
            handler(static_cast<QTcpSocket*>(socket), *static_cast<T*>(message));
        };
    }

    /**
     * @brief Dispatches a message to the appropriate handler.
     *
     * Attempts to find and invoke the registered handler for the message type T. If a handler is found,
     * it is called with the provided QTcpSocket pointer and message data. If no handler is registered for
     * the message type, an error message is printed to std::cerr.
     *
     * @tparam T The type of the message.
     * @param socket Pointer to the QTcpSocket associated with the message.
     * @param data Reference to the message data.
     */
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
    /**
     * @brief Signal emitted to write data.
     *
     * This signal is emitted with a vector of bytes that represent data to be written.
     *
     * @param data A vector of uint8_t containing the data.
     */
    void write_data(std::vector<uint8_t> data);

   private:
    /// Maps message type indices to their corresponding handler functions.
    std::unordered_map<std::type_index, HandlerFunction> handlers;
    /// Indicates whether the message handlers have been initialized.
    bool handlers_initialized = false;
};
