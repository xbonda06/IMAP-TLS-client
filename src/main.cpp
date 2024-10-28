#include <iostream>
#include "../include/ArgParser.h"
#include "../include/IMAPClient.h"
#include "IMAPCommandFactory.h"

int main(int argc, char* argv[]) {
    try {
        ArgParser parser;
        ArgParser::Config config = parser.parse(argc, argv);

        IMAPClient client(config.server, config.port);
        client.connect();
        std::string connectResponse = client.readWholeResponse();
        std::cout << "Command Connect response: " << connectResponse << std::endl;

        auto loginCommand = IMAPCommandFactory::createLoginCommand(config.username, config.server, config.password);
        client.sendCommand(*loginCommand);
        std::string loginResponse = client.readWholeResponse();
        std::cout << "Command LOGIN response: " << loginResponse << std::endl;

        auto selectCommand = IMAPCommandFactory::createSelectCommand(config.mailbox);
        client.sendCommand(*selectCommand);
        std::string selectResponse = client.readWholeResponse();
        std::cout << "Command SELECT response: " << selectResponse << std::endl;

        auto searchCommand = IMAPCommandFactory::createSearchCommand(config.onlyNew);
        client.sendCommand(*searchCommand);
        std::string searchResponse = client.readWholeResponse();
        std::cout << "Command SEARCH response: " << searchResponse << std::endl;

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
