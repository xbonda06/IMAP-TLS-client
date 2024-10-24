//
// Created by xbonda06 on 24.10.2024.
//

#ifndef IMAP_TLS_CLIENT_IMAPCOMMAND_H
#define IMAP_TLS_CLIENT_IMAPCOMMAND_H

#include <string>

// abstract class for commands
class IMAPCommand {
public:
    virtual std::string generate() const = 0;
    virtual ~IMAPCommand() = default;
};

#endif //IMAP_TLS_CLIENT_IMAPCOMMAND_H
