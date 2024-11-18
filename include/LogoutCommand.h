//
// Created by Andrii Bondarenko (xbonda06) on 16.11.2024.
//

#ifndef IMAP_TLS_CLIENT_LOGOUTCOMMAND_H
#define IMAP_TLS_CLIENT_LOGOUTCOMMAND_H

#include "IMAPCommand.h"
#include <string>

/**
 * @brief Represents the IMAP LOGOUT command.
 */
class LogoutCommand : public IMAPCommand {

public:
    LogoutCommand(){}

    std::string generate() const override {
        return "LOGOUT\r\n";
    }

    int getType() const override {return LOGOUT;}
};

#endif //IMAP_TLS_CLIENT_LOGOUTCOMMAND_H
