//
// Created by xbonda06 on 24.10.2024.
//

#ifndef IMAP_TLS_CLIENT_IMAPCOMMAND_H
#define IMAP_TLS_CLIENT_IMAPCOMMAND_H

#include <string>

#define CONNECT 0
#define LOGIN 1
#define SELECT 2
#define SEARCH 3
#define FETCH 4
#define LOGOUT 5

// abstract class for commands
class IMAPCommand {
public:
    virtual std::string generate() const = 0;
    virtual ~IMAPCommand() = default;
    virtual int getType() const = 0;
};

#endif //IMAP_TLS_CLIENT_IMAPCOMMAND_H
