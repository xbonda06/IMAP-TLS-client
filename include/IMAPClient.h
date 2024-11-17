//
// Created by xbonda06 on 24.10.2024.
//

#ifndef IMAP_TLS_CLIENT_IMAPCLIENT_H
#define IMAP_TLS_CLIENT_IMAPCLIENT_H

#include <string>
#include <vector>
#include <openssl/ssl.h>
#include <memory>
#include "IMAPCommand.h"
#include "IMAPResponceType.h"
#include "ArgParser.h"
#include "ConnectionStrategy.h"


class IMAPClient{
public:
    explicit IMAPClient(ArgParser::Config config);

    void connect();

    void login();

    void select();

    bool search();

    void fetch();

    void logout();

    void sendCommand(const IMAPCommand& command);

    [[nodiscard]] std::string readResponse() const;

    std::string readWholeResponse();

    void generateNextTag();

private:
    ArgParser::Config config;
    int currTagNum;
    std::string currTag;
    int lastCommand{};
    int messageSaved = 0;
    std::vector<int> ids;

    std::unique_ptr<ConnectionStrategy> strategy;

    [[nodiscard]] size_t findOk(const std::string& response) const;

    [[nodiscard]] IMAPResponseType findResponseType(const std::string &response) const;

    [[nodiscard]] size_t findNo(const std::string &response) const;

    [[nodiscard]] size_t findBad(const std::string &response) const;

    static std::string decodeBase64(const std::string &encoded);

    static std::string extractAndDecodeSubject(const std::string &headers);

    static std::string validateSubject(const std::string &subject);

    static std::string decodeQuotedPrintable(const std::string &encoded);

    std::string fetchById(int messageNumber);

    [[nodiscard]] bool saveMessage(int messageId, const std::string &messageBody) const;

    size_t processMessage(const std::string &response, int messageId, size_t startPos);
};

#endif //IMAP_TLS_CLIENT_IMAPCLIENT_H
