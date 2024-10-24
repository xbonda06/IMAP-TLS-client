#include <iostream>
#include "../include/ArgParser.h"
#include "../include/IMAPClient.h"

int main(int argc, char* argv[]) {
    try {
        ArgParser parser;
        ArgParser::Config config = parser.parse(argc, argv);

        IMAPClient client(config.server, config.port);
        client.connect();
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }



    return 0;
}
