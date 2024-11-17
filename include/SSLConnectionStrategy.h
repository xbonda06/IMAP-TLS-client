//
// Created by media on 17.11.2024.
//

#ifndef IMAP_TLS_CLIENT_SSLCONNECTIONSTRATEGY_H
#define IMAP_TLS_CLIENT_SSLCONNECTIONSTRATEGY_H

#include "ConnectionStrategy.h"
#include "SSLWrapper.h"
#include <string>
#include <stdexcept>
#include <netdb.h>
#include <unistd.h>

class SSLConnectionStrategy : public ConnectionStrategy {
private:
    SSL* ssl;
    int sockfd;
    std::string server;
    int port;

public:
    SSLConnectionStrategy(const std::string& server, int port)
            : ssl(nullptr), sockfd(-1), server(server), port(port) {}

    void connect() override {
        SSLWrapper::getInstance().initSSL();
        struct sockaddr_in server_addr{};
        struct hostent* host = gethostbyname(server.c_str());
        if (host == nullptr) {
            throw std::runtime_error("Invalid server address");
        }

        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0) {
            throw std::runtime_error("Error creating socket");
        }

        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        server_addr.sin_addr = *((struct in_addr*) host->h_addr);

        if (::connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            close(sockfd);
            throw std::runtime_error("Failed to connect to server");
        }

        ssl = SSLWrapper::getInstance().createSSLConnection(sockfd);
        if (!ssl) {
            throw std::runtime_error("Failed to establish SSL connection");
        }
    }

    void disconnect() override {
        if (ssl) {
            SSLWrapper::getInstance().closeSSLConnection(ssl);
            ssl = nullptr;
        }
        if (sockfd != -1) {
            close(sockfd);
        }
    }

    void sendCommand(const IMAPCommand& command) override {
        std::string cmd = command.generate();
        SSLWrapper::getInstance().sendData(ssl, cmd);
    }

    std::string readResponse() const override {
        std::string response;
        SSLWrapper::getInstance().receiveData(ssl, response);
        return response;
    }
};

#endif //IMAP_TLS_CLIENT_SSLCONNECTIONSTRATEGY_H
