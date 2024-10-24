//
// Created by xbonda06 on 24.10.2024.
//

#include "../include/IMAPClient.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <iostream>
#include <stdexcept>


IMAPClient::IMAPClient(const std::string& server, int port)
        : server(server), port(port), sockfd(-1) {}

void IMAPClient::connect() {
    createTCPConnetction();
}

void IMAPClient::createTCPConnetction() {
    struct sockaddr_in server_addr{};
    struct hostent* host;

    host = gethostbyname(server.c_str());
    if(host == nullptr)
        throw std::runtime_error("Invalid IP-address");

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
        throw std::runtime_error("Creating socket error");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr = *((struct in_addr*) host->h_addr);

    if (::connect(sockfd, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) {
        close(sockfd);
        throw std::runtime_error("Failed connection to the server");
    }
}
