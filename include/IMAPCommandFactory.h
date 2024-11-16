//
// Created by xbonda06 on 24.10.2024.
//

#ifndef IMAP_TLS_CLIENT_IMAPCOMMANDFACTORY_H
#define IMAP_TLS_CLIENT_IMAPCOMMANDFACTORY_H

#include "LoginCommand.h"
#include "FetchCommand.h"
#include "SelectCommand.h"
#include "SearchCommand.h"
#include "LogoutCommand.h"
#include <memory>

class IMAPCommandFactory {
public:
    static std::unique_ptr<IMAPCommand> createLoginCommand(const std::string& user, const std::string& server, const std::string& pass) {
        return std::make_unique<LoginCommand>(user, server, pass);
    }

    static std::unique_ptr<IMAPCommand> createSelectCommand(const std::string& mailbox) {
        return std::make_unique<SelectCommand>(mailbox);
    }

    static std::unique_ptr<IMAPCommand> createSearchCommand(bool onlyNew){
        return std::make_unique<SearchCommand>(onlyNew);
    }

    static std::unique_ptr<IMAPCommand> createFetchCommand(bool onlyHeaders) {
        return std::make_unique<FetchCommand>(onlyHeaders);
    }

    static std::unique_ptr<IMAPCommand> createLogoutCommand() {
        return std::make_unique<LogoutCommand>();
    }
};


#endif //IMAP_TLS_CLIENT_IMAPCOMMANDFACTORY_H
