//
// Created by xbonda06 on 24.10.2024.
//

#ifndef IMAP_TLS_CLIENT_IMAPCLIENT_H
#define IMAP_TLS_CLIENT_IMAPCLIENT_H

#include <string>

class IMAPClient{
public:
    IMAPClient(const std::string& server, int port);

    void connect();

private:
    std::string server;
    int port;
    int sockfd;     //socket descriptor

    void createTCPConnetction();
};

#endif //IMAP_TLS_CLIENT_IMAPCLIENT_H
