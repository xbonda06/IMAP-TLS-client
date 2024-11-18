//
// Created by Andrii Bondarenko (xbonda06) on 17.11.2024.
//

#ifndef IMAP_TLS_CLIENT_SSLCONNECTIONSTRATEGY_H
#define IMAP_TLS_CLIENT_SSLCONNECTIONSTRATEGY_H

#include "ConnectionStrategy.h"
#include "SSLWrapper.h"
#include <string>
#include <stdexcept>
#include <netdb.h>
#include <unistd.h>

/**
 * @brief Implements a strategy for establishing an SSL/TLS connection with the IMAP server.
 *
 * The SSLConnectionStrategy class handles establishing a secure connection to the IMAP server,
 * sending commands, and reading responses over SSL/TLS.
 */
class SSLConnectionStrategy : public ConnectionStrategy {
private:
    SSL* ssl;               ///< Pointer to the SSL connection object.
    int sockfd;             ///< Socket file descriptor.
    std::string server;     ///< The IMAP server address.
    int port;               ///< The server port
    std::string certFile;   ///< Path to the SSL certificate file (optional).
    std::string certDir;    ///< Directory containing SSL certificates (optional).

public:
    SSLConnectionStrategy(const std::string& server, int port, const std::string& certFile = "", const std::string& certDir = "")
            : ssl(nullptr), sockfd(-1), server(server), port(port), certFile(certFile), certDir(certDir) {}

    void connect() override {
        SSLWrapper::getInstance().initSSL();

        if (!certFile.empty()) {
            SSLWrapper::getInstance().setCertificate(certFile);
        }
        if (!certDir.empty()) {
            SSLWrapper::getInstance().setCertDirectory(certDir);
        }

        struct addrinfo hints{};
        struct addrinfo* result;

        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;

        int status = getaddrinfo(server.c_str(), std::to_string(port).c_str(), &hints, &result);
        if (status != 0) {
            throw std::runtime_error("getaddrinfo error: " + std::string(gai_strerror(status)));
        }

        struct addrinfo* p;
        for (p = result; p != nullptr; p = p->ai_next) {
            sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
            if (sockfd < 0) {
                continue;
            }

            if (::connect(sockfd, p->ai_addr, p->ai_addrlen) == 0) {
                break;
            }

            close(sockfd);
        }

        freeaddrinfo(result);

        if (p == nullptr) {
            throw std::runtime_error("Failed to connect to server");
        }

        ssl = SSLWrapper::getInstance().createSSLConnection(sockfd);
        if (!ssl) {
            close(sockfd);
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
            sockfd = -1;
        }
    }

    void sendCommand(std::string command) override {
        SSLWrapper::getInstance().sendData(ssl, command);
    }

    std::string readResponse() const override {
        std::string response;
        SSLWrapper::getInstance().receiveData(ssl, response);
        return response;
    }
};

#endif //IMAP_TLS_CLIENT_SSLCONNECTIONSTRATEGY_H
