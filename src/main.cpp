#include <iostream>
#include "../include/ArgParser.h"
#include "../include/IMAPClient.h"
#include "IMAPCommandFactory.h"

int main(int argc, char* argv[]) {
    try {
        ArgParser parser;
        ArgParser::Config config = parser.parse(argc, argv);

        IMAPClient client(config);

        client.connect();

        client.login();

        client.select();

        client.search();

        auto fetchCommand = IMAPCommandFactory::createFetchCommand(config.onlyHeaders);
        client.sendCommand(*fetchCommand);
        std::string fetchResponse = client.readWholeResponse();
        std::cout << "Command FETCH response: " << fetchResponse << std::endl;

    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
