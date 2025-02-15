#pragma once
#include <QTcpServer>

/**
 * @brief A TCP server that handles incoming connections.
 *
 * The TcpServer class inherits from QTcpServer and overrides the incomingConnection()
 * method to handle new client connections. This class can be extended to provide custom
 * logic for managing incoming connections.
 */
class TcpServer : public QTcpServer {
    Q_OBJECT

   public:
    /**
     * @brief Constructs a new TcpServer instance.
     *
     * Initializes the TcpServer with an optional parent QObject.
     *
     * @param parent The parent QObject (default is nullptr).
     */
    explicit TcpServer(QObject* parent = nullptr);

   protected:
    /**
     * @brief Handles incoming connections.
     *
     * This method is called by QTcpServer when a new connection is available.
     * It receives the socket descriptor for the new connection, allowing derived classes
     * to implement custom logic for handling client connections.
     *
     * @param socketDescriptor The socket descriptor for the incoming connection.
     */
    void incomingConnection(qintptr socketDescriptor) override;
};
