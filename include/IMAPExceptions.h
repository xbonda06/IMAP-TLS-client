//
// Created by Andrii Bondarenko (xbonda06) on 28.10.2024.
//

#ifndef IMAP_TLS_CLIENT_IMAPEXCEPTIONS_H
#define IMAP_TLS_CLIENT_IMAPEXCEPTIONS_H

#include <stdexcept>
#include <string>

class IMAPNoResponseException : public std::runtime_error {
public:
    explicit IMAPNoResponseException(const std::string& message)
            : std::runtime_error("IMAP NO Response: " + message) {}
};

class IMAPBadResponseException : public std::runtime_error {
public:
    explicit IMAPBadResponseException(const std::string& message)
            : std::runtime_error("IMAP BAD Response: " + message) {}
};

#endif //IMAP_TLS_CLIENT_IMAPEXCEPTIONS_H
