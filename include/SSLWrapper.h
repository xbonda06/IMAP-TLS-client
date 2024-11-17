//
// Created by media on 17.11.2024.
//

#ifndef IMAP_TLS_CLIENT_SSLWRAPPER_H
#define IMAP_TLS_CLIENT_SSLWRAPPER_H

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <string>

class SSLWrapper {
public:
    static SSLWrapper& getInstance();

    void initSSL();
    SSL* createSSLConnection(int socket);
    void cleanupSSL();
    void closeSSLConnection(SSL* ssl);

    int sendData(SSL* ssl, const std::string& data);
    int receiveData(SSL* ssl, std::string& buffer);

    void setCertificate(const std::string& certFile);
    void setCertDirectory(const std::string& certDir);

private:
    SSL_CTX* ctx;

    SSLWrapper();
    ~SSLWrapper();

    SSLWrapper(const SSLWrapper&) = delete;
    SSLWrapper& operator=(const SSLWrapper&) = delete;

    void initContext();
};

#endif //IMAP_TLS_CLIENT_SSLWRAPPER_H
