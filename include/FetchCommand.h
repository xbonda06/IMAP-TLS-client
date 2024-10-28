//
// Created by xbonda06 on 24.10.2024.
//

#ifndef IMAP_TLS_CLIENT_FETCHCOMMAND_H
#define IMAP_TLS_CLIENT_FETCHCOMMAND_H

#include "IMAPCommand.h"
#include <string>

class FetchCommand : public IMAPCommand {
    int messageNumber;
    bool onlyHeaders;

public:
    FetchCommand(int messageNumber, bool onlyHeaders) : messageNumber(messageNumber), onlyHeaders(onlyHeaders) {}

    std::string generate() const override {
        std::string fetchPart = onlyHeaders ? "BODY[HEADER]" : "BODY[]";

        return "FETCH " + std::to_string(messageNumber) + " (" + fetchPart + ")\r\n";
    }

    int getType() const override {return FETCH;}
};

#endif //IMAP_TLS_CLIENT_FETCHCOMMAND_H
