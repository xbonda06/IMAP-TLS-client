//
// Created by Andrii Bondarenko (xbonda06) on 27.10.2024.
//

#ifndef IMAP_TLS_CLIENT_SEARCHCOMMAND_H
#define IMAP_TLS_CLIENT_SEARCHCOMMAND_H

#include "IMAPCommand.h"
#include <string>

class SearchCommand : public IMAPCommand {
    bool onlyNew;

public:
    SearchCommand(bool onlyNew) : onlyNew(onlyNew) {}

    std::string generate() const override {
        std::string searchPart = onlyNew ? "UNSEEN" : "ALL";

        return "SEARCH " + searchPart + "\r\n";
    }

    int getType() const override {return SEARCH;}
};

#endif //IMAP_TLS_CLIENT_SEARCHCOMMAND_H
