//
// Created by Andrii Bondarenko (xbonda06) on 17.11.2024.
//

#ifndef IMAP_TLS_CLIENT_CONNECTIONSTRATEGY_H
#define IMAP_TLS_CLIENT_CONNECTIONSTRATEGY_H

#include <string>
#include "IMAPCommand.h"

/**
 * @brief Abstract base class defining the interface for connection strategies.
 *
 * The ConnectionStrategy class is used to define the interface for establishing,
 * disconnecting, sending commands, and reading responses over different types of connections (e.g., TCP or SSL).
 */
class ConnectionStrategy {
public:
    /**
     * @brief Virtual destructor to allow derived classes to clean up resources.
     */
    virtual ~ConnectionStrategy() = default;

    /**
     * @brief Establishes a connection to the server.
     */
    virtual void connect() = 0;

    /**
     * @brief Disconnects from the server and releases any allocated resources.
     */
    virtual void disconnect() = 0;

    /**
     * @brief Sends an IMAP command to the server.
     */
    virtual void sendCommand(std::string command) = 0;

    /**
     * @brief Reads the server's response to the previously sent command.
     */
    virtual std::string readResponse() const = 0;
};

#endif //IMAP_TLS_CLIENT_CONNECTIONSTRATEGY_H
