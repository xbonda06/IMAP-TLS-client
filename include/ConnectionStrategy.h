//
// Created by Andrii Bondarenko (xbonda06) on 17.11.2024.
//

#ifndef IMAP_TLS_CLIENT_CONNECTIONSTRATEGY_H
#define IMAP_TLS_CLIENT_CONNECTIONSTRATEGY_H

#include <string>
#include "IMAPCommand.h"

class ConnectionStrategy {
public:
    virtual ~ConnectionStrategy() = default;

    virtual void connect() = 0;
    virtual void disconnect() = 0;
    virtual void sendCommand(std::string command) = 0;
    virtual std::string readResponse() const = 0;
};

#endif //IMAP_TLS_CLIENT_CONNECTIONSTRATEGY_H
