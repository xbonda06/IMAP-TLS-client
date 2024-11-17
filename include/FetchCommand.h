//
// Created by Andrii Bondarenko (xbonda06) on 24.10.2024.
//

#ifndef IMAP_TLS_CLIENT_FETCHCOMMAND_H
#define IMAP_TLS_CLIENT_FETCHCOMMAND_H

#include "IMAPCommand.h"
#include <string>

class FetchCommand : public IMAPCommand {
    bool onlyHeaders;

public:
    FetchCommand(bool onlyHeaders) : onlyHeaders(onlyHeaders) {}

    std::string generate() const override {
        std::string fetchPart = onlyHeaders ? "BODY[HEADER]" : "BODY[]";

        return "FETCH 1:* (" + fetchPart + ")\r\n";
    }

    int getType() const override {return FETCH;}
};

#endif //IMAP_TLS_CLIENT_FETCHCOMMAND_H
