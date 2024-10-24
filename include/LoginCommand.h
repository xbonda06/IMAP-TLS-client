//
// Created by xbonda06 on 24.10.2024.
//

#ifndef IMAP_TLS_CLIENT_LOGINCOMMAND_H
#define IMAP_TLS_CLIENT_LOGINCOMMAND_H

#include "IMAPCommand.h"
#include <string>

class LoginCommand : public IMAPCommand {
    std::string username;
    std::string password;

public:
    LoginCommand(const std::string& user, const std::string& pass)
            : username(user), password(pass) {}

    std::string generate() const override {
        return "LOGIN " + username + " " + password + "\r\n";
    }
};

#endif //IMAP_TLS_CLIENT_LOGINCOMMAND_H
