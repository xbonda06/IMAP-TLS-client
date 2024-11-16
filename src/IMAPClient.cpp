//
// Created by xbonda06 on 24.10.2024.
//

#include "../include/IMAPClient.h"
#include "IMAPResponceType.h"
#include "IMAPExceptions.h"
#include "ArgParser.h"
#include "IMAPCommandFactory.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <sstream>
#include <regex>
#include <fstream>
#include <filesystem>
#include <openssl/bio.h>
#include <openssl/evp.h>


IMAPClient::IMAPClient(const ArgParser::Config &config)
        : config(config), sockfd(-1), currTagNum(1) {}

void IMAPClient::connect() {
    createTCPConnection();
    readWholeResponse();
}

void IMAPClient::login(){
    auto loginCommand = IMAPCommandFactory::createLoginCommand(config.username, config.server, config.password);
    sendCommand(*loginCommand);
    readWholeResponse();
}

void IMAPClient::select(){
    auto selectCommand = IMAPCommandFactory::createSelectCommand(config.mailbox);
    sendCommand(*selectCommand);
    readWholeResponse();
}

bool IMAPClient::search(){
    auto searchCommand = IMAPCommandFactory::createSearchCommand(config.onlyNew);
    sendCommand(*searchCommand);
    std::string searchResponse = readWholeResponse();

    std::regex searchRegex(R"(\* SEARCH\s((?:\d+\s*)+))");
    std::smatch match;

    if (std::regex_search(searchResponse, match, searchRegex) && match.size() > 1) {
        std::istringstream iss(match[1].str());
        int id;

        while (iss >> id) {
            ids.push_back(id);
        }
        return true;

    } else if (findOk(searchResponse) != std::string::npos){
        return false;
    } else {
        throw std::runtime_error("SEARCH command response does not match with expected");
    }
}

void IMAPClient::fetch() {
    auto fetchCommand = IMAPCommandFactory::createFetchCommand(config.onlyHeaders);

    sendCommand(*fetchCommand);

    std::string response = readWholeResponse();

    std::filesystem::create_directories(config.outDir);

    int savedCount = 0;
    size_t pos = 0;

    // Find the start of each message
    while ((pos = response.find("* ", pos)) != std::string::npos) {
        // Find the id
        size_t idStart = pos + 2;
        size_t idEnd = response.find(' ', idStart);
        if (idEnd == std::string::npos) break;

        int messageId = std::stoi(response.substr(idStart, idEnd - idStart));

        // Check if found id is in uids
        if (std::find(ids.begin(), ids.end(), messageId) == ids.end()) {
            pos = idEnd;
            continue;
        }

        // Find the start of the response body
        size_t bodyStart = response.find("{", idEnd);
        if (bodyStart == std::string::npos) break;
        size_t bodyEnd = response.find("}", bodyStart);
        if (bodyEnd == std::string::npos) break;

        int messageSize = std::stoi(response.substr(bodyStart + 1, bodyEnd - bodyStart - 1));
        size_t messageStart = bodyEnd + 3; // skip "}\r\n"

        std::string messageBody = response.substr(messageStart, messageSize);

        size_t headersEnd = messageBody.find("\r\n\r\n");
        std::string headers = messageBody.substr(0, headersEnd);

        std::string subject = extractAndDecodeSubject(headers);

        subject = validateSubject(subject);

        std::replace(subject.begin(), subject.end(), ' ', '_');

        std::string filename = config.outDir + "/msg_" + std::to_string(messageId) + "_" + subject;
        std::ofstream outFile(filename);
        if (outFile) {
            outFile << messageBody;
            outFile.close();
            savedCount++;
        } else {
            throw std::runtime_error("Failed to create a file to save the message");
        }

        // Update position for the next message search
        pos = messageStart + messageSize + 2;
    }

    std::cout << "Saved " << savedCount << " messages from the " << config.mailbox << "." << std::endl;
}

void IMAPClient::logout() {
    auto logoutCommand = IMAPCommandFactory::createLogoutCommand();
    sendCommand(*logoutCommand);
    readWholeResponse();
}

void IMAPClient::generateNextTag(){
    currTag = "A" + std::to_string(currTagNum++);
}

void IMAPClient::createTCPConnection() {
    struct sockaddr_in server_addr{};
    struct hostent* host;

    host = gethostbyname(config.server.c_str());
    if(host == nullptr)
        throw std::runtime_error("Invalid IP-address");

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
        throw std::runtime_error("Creating socket error");

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(config.port);
    server_addr.sin_addr = *((struct in_addr*) host->h_addr);

    if (::connect(sockfd, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) {
        close(sockfd);
        throw std::runtime_error("Failed connection to the server");
    } else {
        lastCommand = CONNECT;
    }
}

void IMAPClient::sendCommand(const IMAPCommand& command) {
    generateNextTag();
    std::string cmdStr = currTag + " " + command.generate();
    lastCommand = command.getType();
    if (send(sockfd, cmdStr.c_str(), cmdStr.size(), 0) < 0) {
        throw std::runtime_error("Failed IMAP command sending");
    }
}

std::string IMAPClient::readResponse() {
    char buffer[1024];

    ssize_t bytesRead = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead < 0) {
        throw std::runtime_error("Failed to read response");
    }
    buffer[bytesRead] = '\0';
    std::string response(buffer);

    return response;
}

std::string IMAPClient::readWholeResponse() {
    bool response_found = false;
    std::string lastMessage;
    std::string finalMessage;
    IMAPResponseType responseType;

    while (!response_found) {
        lastMessage = readResponse();
        responseType = findResponseType(lastMessage);

        if (responseType == IMAPResponseType::OK || responseType == IMAPResponseType::NO || responseType == IMAPResponseType::BAD) {
            response_found = true;
        }

        finalMessage.append(lastMessage);
    }

    if (responseType == IMAPResponseType::NO) {
        throw IMAPNoResponseException(finalMessage);
    } else if (responseType == IMAPResponseType::BAD) {
        throw IMAPBadResponseException(finalMessage);
    }

    return finalMessage;
}

size_t IMAPClient::findOk(const std::string& response) const {
    size_t idx;

    switch (lastCommand) {
        case CONNECT:
        case LOGIN:
            idx = response.find("OK");
            break;
        case SELECT:
            idx = response.find("OK [READ-WRITE] Select completed");
            break;
        case SEARCH:
            idx = response.find("OK Search completed");
            break;
        case FETCH:
            idx = response.find("OK Fetch completed");
            break;
        case LOGOUT:
            idx = response.find("OK Logout completed");
            break;
        default:
            break;
    }
    return idx;
}

size_t IMAPClient::findNo(const std::string& response) const {
    size_t idx;

    switch (lastCommand) {
        case LOGIN:
            idx = response.find("NO [AUTHENTICATIONFAILED] Authentication failed");
            break;
        case SELECT:
            idx = response.find("NO Mailbox doesn't exist");
            break;
        default:
            return std::string::npos;
    }
    return idx;
}

size_t IMAPClient::findBad(const std::string& response) const {
    size_t idx;

    switch (lastCommand) {
        case SEARCH:
            idx = response.find("BAD Error in IMAP command SEARCH");
            break;
        case FETCH:
            idx = response.find("BAD Error in IMAP command FETCH");
            break;
        default:
            return std::string::npos;
    }
    return idx;
}

IMAPResponseType IMAPClient::findResponseType(const std::string& response) const {
    if (findOk(response) != std::string::npos) {
        return IMAPResponseType::OK;
    } else if (findNo(response) != std::string::npos) {
        return IMAPResponseType::NO;
    } else if (findBad(response) != std::string::npos) {
        return IMAPResponseType::BAD;
    }
    return IMAPResponseType::UNKNOWN;
}

std::string IMAPClient::decodeBase64(const std::string &encoded) {
    BIO *bio, *b64;
    char buffer[1024];
    std::string decoded;

    bio = BIO_new_mem_buf(encoded.data(), encoded.size());
    b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    bio = BIO_push(b64, bio);

    int decodedLength;
    while ((decodedLength = BIO_read(bio, buffer, sizeof(buffer))) > 0) {
        decoded.append(buffer, decodedLength);
    }

    BIO_free_all(bio);
    return decoded;
}

std::string IMAPClient::decodeQuotedPrintable(const std::string &encoded) {
    std::ostringstream decoded;
    for (size_t i = 0; i < encoded.size(); ++i) {
        if (encoded[i] == '=' && i + 2 < encoded.size()) {
            std::string hex = encoded.substr(i + 1, 2);
            char decodedChar = static_cast<char>(std::stoi(hex, nullptr, 16));
            decoded << decodedChar;
            i += 2;
        } else {
            decoded << encoded[i];
        }
    }
    return decoded.str();
}

std::string IMAPClient::extractAndDecodeSubject(const std::string &headers) {
    std::regex encodedSubjectRegex(R"(Subject:\s=\?([A-Za-z0-9-]+)\?(B|Q)\?([A-Za-z0-9+/=]+)\?=)", std::regex::icase);
    std::smatch match;

    if (std::regex_search(headers, match, encodedSubjectRegex)) {
        std::string charset = match[1].str();
        std::string encoding = match[2].str();
        std::string encodedSubject = match[3].str();

        if (encoding == "B" || encoding == "b") {
            return decodeBase64(encodedSubject);
        } else if (encoding == "Q" || encoding == "q") {
            return decodeQuotedPrintable(encodedSubject);
        }
    }

    std::regex plainSubjectRegex(R"(Subject:\s(.+))", std::regex::icase);
    if (std::regex_search(headers, match, plainSubjectRegex)) {
        return match[1].str();
    }

    return "no_subject";
}

std::string IMAPClient::validateSubject(const std::string &subject) {
    std::string cleanSubject;
    for (char c : subject) {
        if (c != '/' && c != '\\' && c != ':' && c != '*' && c != '?' &&
            c != '"' && c != '<' && c != '>' && c != '|' && c != '&' &&
            c != ';' && c != ',' && c != '.') {
            cleanSubject += c;
        }
    }
    return cleanSubject;
}