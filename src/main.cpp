//
// Created by Andrii Bondarenko (xbonda06)
//
#include <iostream>
#include "../include/ArgParser.h"
#include "../include/IMAPClient.h"
#include "IMAPCommandFactory.h"
#include "SSLWrapper.h"

int main(int argc, char* argv[]) {
    try {
        ArgParser parser;
        ArgParser::Config config = parser.parse(argc, argv);

        config.port = config.useSSL ? 993 : 143;

        IMAPClient client(config);

        client.connect();

        client.login();

        client.select();

        if(client.search()){
            client.fetch();
        } else {
            std::cout << "No message has been downloaded from the " + config.mailbox + " mailbox" << std::endl;
        }

        client.logout();

        if (config.useSSL) {
            SSLWrapper::getInstance().cleanupSSL();
        }

    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }


    
    return 0;
}
