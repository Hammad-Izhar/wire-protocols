#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <iostream>

#include "models/message_handler.hpp"
#include "server/model/tcp_server.hpp"

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

    int port = jsonObj["port"].toInt();

    // Start the TCP server
    TcpServer server;
    if (!server.listen(QHostAddress::Any, port)) {
        std::cerr << "TCP Server failed to start: " << server.errorString().toStdString()
                  << std::endl;
        return -1;
    }

    std::cout << "Server started on port " << port << std::endl;
    return app.exec();
}
