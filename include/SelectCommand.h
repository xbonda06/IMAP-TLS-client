//
// Created by xbonda06 on 25.10.2024.
//

#ifndef IMAP_TLS_CLIENT_SELECTCOMMAND_H
#define IMAP_TLS_CLIENT_SELECTCOMMAND_H

#include "IMAPClient.h"
#include <string>

class SelectCommand : public IMAPCommand {
public:
    explicit SelectCommand(const std::string& mailbox) : mailbox(mailbox) {}

    std::string generate() const override {
        return "SELECT " + mailbox + "\r\n";
    }

private:
    std::string mailbox;
};

#endif //IMAP_TLS_CLIENT_SELECTCOMMAND_H
