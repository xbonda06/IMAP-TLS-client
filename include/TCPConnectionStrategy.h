//
// Created by media on 17.11.2024.
//

#ifndef IMAP_TLS_CLIENT_TCPCONNECTIONSTRATEGY_H
#define IMAP_TLS_CLIENT_TCPCONNECTIONSTRATEGY_H

#include "ConnectionStrategy.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <stdexcept>

class TCPConnectionStrategy : public ConnectionStrategy {
private:
    int sockfd;
    std::string server;
    int port;

public:
    TCPConnectionStrategy(const std::string& server, int port)
            : sockfd(-1), server(server), port(port) {}

    void connect() override {
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
    }

    void disconnect() override {
        if (sockfd != -1) {
            close(sockfd);
        }
    }

    void sendCommand(std::string command) override {
        if (send(sockfd, command.c_str(), command.size(), 0) < 0) {
            throw std::runtime_error("Failed to send command");
        }
    }

    std::string readResponse() const override {
        char buffer[1024];
        ssize_t bytesRead = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead < 0) {
            throw std::runtime_error("Failed to read response");
        }
        buffer[bytesRead] = '\0';
        return std::string(buffer);
    }
};

#endif //IMAP_TLS_CLIENT_TCPCONNECTIONSTRATEGY_H
