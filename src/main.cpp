#include <iostream>
#include "../include/ArgParser.h"

int main(int argc, char* argv[]) {
    ArgParser parser;

    try {
        ArgParser::Config config = parser.parse(argc, argv);

    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
