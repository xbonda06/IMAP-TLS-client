//
// Created by xbonda06 on 24.10.2024.
//

#ifndef IMAP_TLS_CLIENT_IMAPCLIENT_H
#define IMAP_TLS_CLIENT_IMAPCLIENT_H

#include <string>
#include "IMAPCommand.h"

class IMAPClient{
public:
    IMAPClient(const std::string& server, int port);

    void connect();

    void sendCommand(const IMAPCommand& command);

    std::string readResponse();

    void generateNextTag();

private:
    std::string server;
    int port;
    int sockfd;     //socket descriptor
    int currTagNum;
    std::string currTag;

    void createTCPConnetction();
};

#endif //IMAP_TLS_CLIENT_IMAPCLIENT_H
