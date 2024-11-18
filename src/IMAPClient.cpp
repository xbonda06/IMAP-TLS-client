//
// Created by Andrii Bondarenko (xbonda06) on 24.10.2024.
//

#include "../include/IMAPClient.h"
#include "IMAPResponceType.h"
#include "IMAPExceptions.h"
#include "ArgParser.h"
#include "IMAPCommandFactory.h"
#include "SSLWrapper.h"
#include "ConnectionStrategy.h"
#include "SSLConnectionStrategy.h"
#include "TCPConnectionStrategy.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>
#include <sstream>
#include <regex>
#include <fstream>
#include <filesystem>
#include <openssl/bio.h>
#include <openssl/evp.h>

/**
 * @brief Constructs an IMAPClient with specified configuration.
 *
 * Initializes the connection strategy (SSL/TLS or TCP) based on the config.
 * @param config Configuration struct containing server details, SSL settings, and other options.
 */
IMAPClient::IMAPClient(ArgParser::Config config)
        : config(config), currTagNum(1) {

    if (config.useSSL) {
        strategy = std::make_unique<SSLConnectionStrategy>(
                config.server, config.port,
                config.cert.empty() ? "" : config.certDir + "/" + config.cert,
                config.certDir
        );
    } else {
        strategy = std::make_unique<TCPConnectionStrategy>(config.server, config.port);
    }
}

/**
 * @brief Establishes a connection to the IMAP server using the selected strategy.
 *
 * Initiates the connection, then reads the initial server response to ensure connectivity.
 * @throws std::runtime_error if the connection fails.
 */
void IMAPClient::connect() {
    strategy->connect();
    lastCommand = CONNECT;
    readWholeResponse();
}


void IMAPClient::generateNextTag(){
    currTag = "A" + std::to_string(currTagNum++);
}

/**
 * @brief Sends the LOGIN command to authenticate with the IMAP server.
 */
void IMAPClient::login(){
    auto loginCommand = IMAPCommandFactory::createLoginCommand(config.username, config.server, config.password);
    sendCommand(*loginCommand);
    readWholeResponse();
}

/**
 * @brief Sends the SELECT command to choose a mailbox (e.g., INBOX) for further actions.
 */
void IMAPClient::select(){
    auto selectCommand = IMAPCommandFactory::createSelectCommand(config.mailbox);
    sendCommand(*selectCommand);
    readWholeResponse();
}

/**
 * @brief Executes the SEARCH command based on the user's options to retrieve message IDs.
 *
 * This command searches for new or all messages and stores their IDs in a list.
 * @return True if messages matching the criteria were found; otherwise, false.
 * @throws std::runtime_error if the search command fails.
 */
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

/**
 * @brief Fetches messages from the server and saves them to the output directory.
 *
 * If the `onlyNew` option is enabled, it fetches messages one by one; otherwise,
 * it fetches all messages in bulk. Each message is then saved individually.
 */
void IMAPClient::fetch() {
    std::filesystem::create_directories(config.outDir);

    if (config.onlyNew) {
        // Fetch messages one by one for new messages only
        for (int id : ids) {
            std::string response = fetchById(id);
            processMessage(response, id, 0);
        }
    } else {
        // Fetch all messages in bulk
        auto fetchCommand = IMAPCommandFactory::createFetchCommand(config.onlyHeaders);
        sendCommand(*fetchCommand);

        std::string response = readWholeResponse();
        size_t pos = 0;

        // Parse and save messages
        while ((pos = response.find("* ", pos)) != std::string::npos) {
            size_t idStart = pos + 2;
            size_t idEnd = response.find(' ', idStart);
            if (idEnd == std::string::npos) break;

            int messageId = std::stoi(response.substr(idStart, idEnd - idStart));

            // Check if found ID is in the list of IDs
            if (std::find(ids.begin(), ids.end(), messageId) == ids.end()) {
                pos = idEnd;
                continue;
            }

            pos = processMessage(response, messageId, pos);
        }
    }

    if(messageSaved > 0)
        std::cout << "Saved " << messageSaved << " messages from the " << config.mailbox << "." << std::endl;
    else
        std::cout << "No message saved from the " << config.mailbox << "." << std::endl;
}

/**
 * @brief Processes a single message response from the server and saves the message.
 *
 * This function extracts the message body from the response based on the provided start position.
 * It saves the message to a file with a formatted name if the message is valid.
 *
 * @param response The server response containing the message data.
 * @param messageId The unique ID of the message being processed.
 * @param startPos The position in the response to start searching for the message body.
 * @return The position in the response after processing the current message.
 */
size_t IMAPClient::processMessage(const std::string &response, int messageId, size_t startPos) {
    // locate the start of the message body using '{' character
    size_t bodyStart = response.find('{', startPos);
    size_t bodyEnd = response.find('}', bodyStart);

    // if either '{' or '}' is not found, return the original start position to skip invalid responses
    if (bodyStart == std::string::npos || bodyEnd == std::string::npos) {
        return startPos;
    }

    // extract the size of the message body enclosed in curly braces, e.g., {12345}
    int messageSize = std::stoi(response.substr(bodyStart + 1, bodyEnd - bodyStart - 1));
    size_t messageStart = bodyEnd + 3; // Skip "}\r\n"

    // extract the message content from the response based on the size.
    std::string messageBody = response.substr(messageStart, messageSize);

    if (saveMessage(messageId, messageBody)) {
        messageSaved++;
    }

    return messageStart + messageSize + 2; // Move to next position
}

/**
 * @brief Fetches a specific message by its ID using the FETCH command.
 *
 * @param messageNumber The ID of the message to fetch.
 * @return The complete server response containing the message data.
 */
std::string IMAPClient::fetchById(int messageNumber) {
    auto fetchCommand = IMAPCommandFactory::createFetchByIdCommand(messageNumber, config.onlyHeaders);
    sendCommand(*fetchCommand);
    return readWholeResponse();
}

/**
 * @brief Saves a message to a file in the specified output directory.
 *
 * The file is named using the format `msg_<messageId>_<subject>`.
 * The subject is extracted from the message headers and sanitized to remove invalid characters.
 *
 * @param messageId The unique ID of the message.
 * @param messageBody The full content of the message, including headers and body.
 * @return True if the message was saved successfully; otherwise, false.
 */
bool IMAPClient::saveMessage(int messageId, const std::string& messageBody) const {
    // locate the end of the headers section, marked by a blank line
    size_t headersEnd = messageBody.find("\r\n\r\n");
    std::string headers = messageBody.substr(0, headersEnd);

    // extract and decode the subject from the header
    std::string subject = extractAndDecodeSubject(headers);
    subject = validateSubject(subject);
    std::replace(subject.begin(), subject.end(), ' ', '_');

    std::string filename = config.outDir + "/msg_" + std::to_string(messageId) + "_" + subject;

    if (std::filesystem::exists(filename))
        return false;

    std::ofstream outFile(filename);
    if (outFile) {
        outFile << messageBody;
        outFile.close();
        return true;
    } else {
        std::cerr << "Failed to save message " << messageId << " to " << filename << std::endl;
        return false;
    }
}

/**
 * @brief Sends the LOGOUT command and disconnects from the server.
 */
void IMAPClient::logout() {
    auto logoutCommand = IMAPCommandFactory::createLogoutCommand();
    sendCommand(*logoutCommand);
    readWholeResponse();

    strategy->disconnect();
}

/**
 * @brief Sends an IMAP command using the current connection strategy.
 * @param command The IMAP command to send.
 */
void IMAPClient::sendCommand(const IMAPCommand& command) {
    generateNextTag();
    lastCommand = command.getType();
    std::string cmdStr = currTag + " " + command.generate();

    strategy->sendCommand(cmdStr);
}

/**
 * @brief Reads a single response line from the server.
 * @return The response line as a string.
 */
std::string IMAPClient::readResponse() const {
    return strategy->readResponse();
}

/**
 * @brief Reads the complete response from the server until an OK, NO, or BAD response is found.
 *
 * This method collects all lines from the server until a final status response is received.
 * It handles responses that span multiple lines.
 *
 * @return The full response as a string.
 * @throws IMAPNoResponseException if a NO response is received.
 * @throws IMAPBadResponseException if a BAD response is received.
 */
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

/**
 * @brief Finds the position of an "OK" response based on the last command sent.
 */
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

/**
 * @brief Finds the position of a "NO" response based on the last command sent.
 */
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

/**
 * @brief Finds the position of a "BAD" response based on the last command sent.
 */
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

/**
 * @brief Determines the response type (OK, NO, BAD) based on the server's response.
 */
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

/**
 * @brief Decodes a base64-encoded string.
 *
 * This function uses OpenSSL to decode a base64 string and return the decoded data.
 * @param encoded The base64 encoded string.
 * @return The decoded string.
 */
std::string IMAPClient::decodeBase64(const std::string &encoded) {
    BIO *bio, *b64;
    char buffer[1024];
    std::string decoded;

    // set up a memory BIO to read the base64 data
    bio = BIO_new_mem_buf(encoded.data(), encoded.size());
    b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL); // disable newlines
    bio = BIO_push(b64, bio);

    // read decoded data into buffer
    int decodedLength;
    while ((decodedLength = BIO_read(bio, buffer, sizeof(buffer))) > 0) {
        decoded.append(buffer, decodedLength);
    }
    BIO_free_all(bio);
    return decoded;
}

/**
 * @brief Decodes a quoted-printable encoded string.
 *
 * @param encoded The quoted-printable encoded string.
 * @return The decoded string.
 */
std::string IMAPClient::decodeQuotedPrintable(const std::string &encoded) {
    std::ostringstream decoded;
    for (size_t i = 0; i < encoded.size(); ++i) {
        // check for "=XX" pattern representing hex-encoded characters
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

/**
 * @brief Extracts and decodes the subject line from email headers.
 *
 * Handles both base64 and quoted-printable encoded subjects.
 * @param headers The email headers.
 * @return The decoded subject or "no_subject" if not found.
 */
std::string IMAPClient::extractAndDecodeSubject(const std::string &headers) {
    std::regex encodedSubjectRegex(R"(Subject:\s=\?([A-Za-z0-9-]+)\?(B|Q)\?([A-Za-z0-9+/=]+)\?=)", std::regex::icase);
    std::smatch match;

    // check if the subject is encoded
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

    // extract plain text subject if not encoded
    std::regex plainSubjectRegex(R"(Subject:\s(.+))", std::regex::icase);
    if (std::regex_search(headers, match, plainSubjectRegex)) {
        return match[1].str();
    }
    return "no_subject";
}

/**
 * @brief Validates and sanitizes the subject to be used as a filename.
 *
 * Removes any characters that are not allowed in filenames.
 */
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