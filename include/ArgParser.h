//
// Created by xbonda06 on 23.10.2024.
//

#ifndef IMAP_TLS_CLIENT_ARGPARSER_H
#define IMAP_TLS_CLIENT_ARGPARSER_H

#include <string>

class ArgParser{
public:
    struct Config{
        std::string server;
        int port = 143; // imap default port
        bool useSSL = false;
        std::string cert;
        std::string certDir = "/etc/ssl/certs";
        bool onlyNew = false;
        bool onlyHeaders = false;
        std::string authFile;
        std::string mailbox = "INBOX";
        std::string outDir;
    };

    Config parse(int argc, char* argv[]);
};

#endif //IMAP_TLS_CLIENT_ARGPARSER_H
