//
// Created by xbonda06 on 24.10.2024.
//

#ifndef IMAP_TLS_CLIENT_IMAPCLIENT_H
#define IMAP_TLS_CLIENT_IMAPCLIENT_H

#include <string>
#include "IMAPCommand.h"
#include "IMAPResponceType.h"


class IMAPClient{
public:
    IMAPClient(const std::string& server, int port);

    void connect();

    void sendCommand(const IMAPCommand& command);

    std::string readResponse();

    std::string readWholeResponse();

    void generateNextTag();

private:
    std::string server;
    int port;
    int sockfd;     //socket descriptor
    int currTagNum;
    std::string currTag;
    int lastCommand{};

    void createTCPConnection();

    size_t findOk(const std::string& response) const;

    IMAPResponseType findResponseType(const std::string &response) const;

    size_t findNo(const std::string &response) const;

    size_t findBad(const std::string &response) const;
};

#endif //IMAP_TLS_CLIENT_IMAPCLIENT_H
