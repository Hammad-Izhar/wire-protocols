#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <iostream>

#ifdef PROTOCOL_RPC
#include <google/protobuf/message.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/impl/codegen/server_interceptor.h>
#include "server/model/socket_out_impl.hpp"
#endif

#include "models/message_handler.hpp"
#ifndef PROTOCOL_RPC
#include "server/model/tcp_server.hpp"
#endif

#ifdef PROTOCOL_RPC
#include <google/protobuf/message.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/impl/codegen/server_interceptor.h>
#include <iostream>

class MessageSizeInterceptor : public grpc::experimental::Interceptor {
   public:
    explicit MessageSizeInterceptor(grpc::experimental::ServerRpcInfo* info) : info_(info) {}

    void Intercept(grpc::experimental::InterceptorBatchMethods* methods) override {
        // Check for the received message hook point.
        if (methods->QueryInterceptionHookPoint(
                grpc::experimental::InterceptionHookPoints::POST_RECV_MESSAGE)) {
            void* msg = methods->GetRecvMessage();
            if (msg != nullptr) {
                auto* request = static_cast<google::protobuf::Message*>(msg);
                size_t size = request->ByteSizeLong();
                std::cout << "Received message size: " << size << " bytes." << std::endl;
            }
        }

        // Check for the send message hook point.
        if (methods->QueryInterceptionHookPoint(
                grpc::experimental::InterceptionHookPoints::PRE_SEND_MESSAGE)) {
            const void* msg = methods->GetSendMessage();
            if (msg != nullptr) {
                auto* response = static_cast<const google::protobuf::Message*>(msg);
                size_t size = response->ByteSizeLong();
                std::cout << "Sending message size: " << size << " bytes." << std::endl;
            }
        }

        // Continue processing the RPC.
        methods->Proceed();
    }

   private:
    grpc::experimental::ServerRpcInfo* info_;
};

class MessageSizeInterceptorFactory : public grpc::experimental::ServerInterceptorFactoryInterface {
   public:
    grpc::experimental::Interceptor* CreateServerInterceptor(
        grpc::experimental::ServerRpcInfo* info) override {
        return new MessageSizeInterceptor(info);
    }
};
#endif

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
    QCommandLineParser parser;

    parser.setApplicationDescription("Sock-et Out");
    parser.addHelpOption();

    // Define command-line option for config file path
    QCommandLineOption configOption("config", "Path to JSON config file", "file");
    parser.addOption(configOption);

    // Parse command-line arguments
    parser.process(app);

    QString configFilePath;
    if (parser.isSet(configOption)) {
        configFilePath = parser.value(configOption);
        qDebug() << "Using config file:" << configFilePath;
    } else {
        std::cerr << "Error: No config file provided. Use --config <file_path>" << std::endl;
        return -1;
    }

    // Open the JSON file
    QFile configFile(configFilePath);
    if (!configFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        std::cerr << "Error: Could not open file: " << configFilePath.toStdString() << std::endl;
        return -1;
    }

    // Read and parse the JSON file
    QByteArray jsonData = configFile.readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    if (jsonDoc.isNull() || !jsonDoc.isObject()) {
        std::cerr << "Error: Invalid JSON format in " << configFilePath.toStdString() << std::endl;
        return -1;
    }

    // Extract the "port" field
    QJsonObject jsonObj = jsonDoc.object();
    if (!jsonObj.contains("port") || !jsonObj["port"].isDouble()) {
        std::cerr << "Error: 'port' field missing or invalid in JSON." << std::endl;
        return -1;
    }

    if (!jsonObj.contains("db") || !jsonObj["db"].isString()) {
        std::cerr << "Error: 'db' field missing or invalid in JSON." << std::endl;
        return -1;
    }

    int port = jsonObj["port"].toInt();
    std::string db = jsonObj["db"].toString().toStdString();

    std::cout << "'Connecting' to database at: " << db << std::endl;
    Database& database = Database::get_instance(db);

#ifdef PROTOCOL_RPC
    std::cout << "Starting gRPC server on port " << port << std::endl;
    std::string server_address = "0.0.0.0:" + std::to_string(port);
    SocketOutImpl public_service;

    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&public_service);

#ifdef GRPC_INTERCEPTOR
    std::vector<std::unique_ptr<grpc::experimental::ServerInterceptorFactoryInterface>>
        interceptor_creators;
    interceptor_creators.push_back(
        std::unique_ptr<grpc::experimental::ServerInterceptorFactoryInterface>(
            new MessageSizeInterceptorFactory()));
    builder.experimental().SetInterceptorCreators(std::move(interceptor_creators));
#endif

    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Server started on port " << port << std::endl;
    server->Wait();
#else
    // Start the TCP server
    TcpServer server;
    if (!server.listen(QHostAddress::Any, port)) {
        std::cerr << "TCP Server failed to start: " << server.errorString().toStdString()
                  << std::endl;
        return -1;
    }
#endif

    std::cout << "Server started on port " << port << std::endl;
    return app.exec();
}
