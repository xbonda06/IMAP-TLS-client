//
// Created by Andrii Bondarenko (xbonda06) on 23.10.2024.
//

#include "../include/ArgParser.h"
#include <getopt.h>
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <tuple>

ArgParser::Config ArgParser::parse(int argc, char* argv[]){
    Config config;

    if(argv[1][0] != '-'){                      // if first argument is server name
        config.server = argv[1];
        for (int i = 1; i < argc - 1; ++i) {    // move all args left
            argv[i] = argv[i + 1];
        }
    } else {                                    //if first argument is '-' flag
        for(int i = 2; i < argc ; i++){
            if(argv[i][0] != '-' && (argv[i-1][0] != '-' ||      // if current param AND prev param are not '-' params
            argv[i-1][1] == 'T' || argv[i-1][1] == 'n' || argv[i-1][1] == 'h')){ // or if prev param is not '-': param
                config.server = argv[i];

                // move all args left
                for (int j = i; j < argc - 1; ++j) {
                    argv[j] = argv[j + 1];
                }
                argc--;
                break;
            }
        }
    }

    if (config.server.empty()) {
        throw std::invalid_argument("server name is empty");
    }

    int opt;
    while((opt = getopt(argc, argv, "p:Tc:C:nha:b:o:")) != -1){
        switch (opt) {
            case 'p':
                config.port = std::stoi(optarg);
                break;
            case 'T':
                config.useSSL = true;
                break;
            case 'c':
                config.cert = optarg;
                break;
            case 'C':
                config.certDir = optarg;
                break;
            case 'n':
                config.onlyNew = true;
                break;
            case 'h':
                config.onlyHeaders = true;
                break;
            case 'a':
                config.authFile = optarg;
                break;
            case 'b':
                config.mailbox = optarg;
                break;
            case 'o':
                config.outDir = optarg;
                break;
            default:
                throw std::invalid_argument("invalid argument");
        }
    }

    if (config.authFile.empty() || config.outDir.empty()) {
        throw std::invalid_argument("Required params: -a (auth_file) -o (output_dir)");
    }

    std::tie(config.username, config.password) = readAuthFile(config.authFile);

    return config;
}

std::pair<std::string, std::string> ArgParser::readAuthFile(const std::string &authFilePath) {
    std::ifstream authFile(authFilePath);
    if (!authFile.is_open()) {
        throw std::runtime_error("Failed to open auth file");
    }

    std::string line, username, password;
    if (std::getline(authFile, line)) {
        std::istringstream iss(line);
        std::string key, value;

        if (!(iss >> key) || key != "username" || !(iss >> key) || key != "=" || !(iss >> username)) {
            throw std::runtime_error("Incorrect username format in auth file");
        }

        if (!(iss >> key) || key != "password" || !(iss >> key) || key != "=" || !(iss >> password)) {
            throw std::runtime_error("Incorrect password format in auth file");
        }

        return {username, password};
    }

    throw std::runtime_error("Bad auth file format");
}

