//
// Created by Andrii Bondarenko (xbonda06) on 23.10.2024.
//

#ifndef IMAP_TLS_CLIENT_ARGPARSER_H
#define IMAP_TLS_CLIENT_ARGPARSER_H

#include <string>

/**
 * @brief The ArgParser class is responsible for parsing command-line arguments
 *        and storing them in a Config structure for use by the IMAP client.
 */
class ArgParser{
public:

    /**
     * @brief Configuration structure that holds all necessary parameters for the IMAP client.
     */
    struct Config{
        std::string server;
        int port = useSSL ? 993 : 143; // imap default port
        bool useSSL = false;
        std::string cert;
        std::string certDir = "/etc/ssl/certs";
        bool onlyNew = false;
        bool onlyHeaders = false;
        std::string authFile;
        std::string mailbox = "INBOX";
        std::string outDir;
        std::string username;
        std::string password;
    };

    Config parse(int argc, char* argv[]);

private:
    std::pair<std::string, std::string> readAuthFile(const std::string& authFilePath);
};

#endif //IMAP_TLS_CLIENT_ARGPARSER_H
