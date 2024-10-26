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
#include <vector>
#include <sstream>


IMAPClient::IMAPClient(const std::string& server, int port)
        : server(server), port(port), sockfd(-1), currTagNum(1) {}

void IMAPClient::connect() {
    createTCPConnetction();
}

void IMAPClient::generateNextTag(){
    currTag = "A" + std::to_string(currTagNum++);
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

void IMAPClient::sendCommand(const IMAPCommand& command) {
    generateNextTag();
    std::string cmdStr = currTag + " " + command.generate();
    if (send(sockfd, cmdStr.c_str(), cmdStr.size(), 0) < 0) {
        throw std::runtime_error("Failed IMAP command sending");
    }
}

std::string IMAPClient::readResponse() {
    char buffer[1024];
    std::string accumulatedResponse;
    std::string finalResponse;

    while (true) {
        ssize_t bytesRead = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead < 0) {
            throw std::runtime_error("Failed to read response");
        }
        buffer[bytesRead] = '\0';
        accumulatedResponse += buffer;

        std::istringstream responseStream(accumulatedResponse);
        std::string line;
        std::vector<std::string> lines;

        while (std::getline(responseStream, line, '\n')) {
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            lines.push_back(line);
        }

        accumulatedResponse.clear();

        for (const auto& line : lines) {
            if (line[0] == '*') {
                std::cout << "Intermediate response: " << line << std::endl;
            } else if (line.find(currTag) == 0) {
                finalResponse = line;
                return finalResponse;
            }
        }
    }
}
