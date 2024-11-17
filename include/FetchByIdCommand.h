//
// Created by Andrii Bondarenko (xbonda06) on 17.11.2024.
//

#ifndef IMAP_TLS_CLIENT_FETCHBYIDCOMMAND_H
#define IMAP_TLS_CLIENT_FETCHBYIDCOMMAND_H

#include "IMAPCommand.h"
#include <string>

class FetchByIdCommand : public IMAPCommand {
    int messageId;
    bool onlyHeaders;

public:
    FetchByIdCommand(int messageId, bool onlyHeaders) : messageId(messageId), onlyHeaders(onlyHeaders) {}

    std::string generate() const override {
        std::string fetchPart = onlyHeaders ? "BODY[HEADER]" : "BODY[]";

        return "FETCH " + std::to_string(messageId) + " (" + fetchPart + ")\r\n";
    }

    int getType() const override {return FETCH;}
};

#endif //IMAP_TLS_CLIENT_FETCHBYIDCOMMAND_H
