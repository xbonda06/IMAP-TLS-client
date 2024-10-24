//
// Created by xbonda06 on 24.10.2024.
//

#ifndef IMAP_TLS_CLIENT_FETCHCOMMAND_H
#define IMAP_TLS_CLIENT_FETCHCOMMAND_H

#include "IMAPCommand.h"
#include <string>

class FetchCommand : public IMAPCommand {
    std::string mailbox;

public:
    FetchCommand(const std::string& mailbox) : mailbox(mailbox) {}

    std::string generate() const override {
        return "FETCH " + mailbox + " (BODY[])\r\n";
    }
};

#endif //IMAP_TLS_CLIENT_FETCHCOMMAND_H
