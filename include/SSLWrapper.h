//
// Created by Andrii Bondarenko (xbonda06) on 17.11.2024.
//

#ifndef IMAP_TLS_CLIENT_SSLWRAPPER_H
#define IMAP_TLS_CLIENT_SSLWRAPPER_H

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <string>

class SSLWrapper {
public:
    /**
     * @brief Singleton instance of SSLWrapper.
     * @return A reference to the single instance of SSLWrapper.
     */
    static SSLWrapper& getInstance();

    /**
     * @brief Initializes the SSL library and loads necessary algorithms and error strings.
     */
    void initSSL();

    /**
     * @brief Creates an SSL connection over an existing TCP socket.
     * @param socket The file descriptor of the TCP socket.
     * @return Pointer to the SSL structure, or nullptr if the connection fails.
     */
    SSL* createSSLConnection(int socket);

    /**
     * @brief Cleans up the SSL context.
     */
    void cleanupSSL();

    /**
     * @brief Closes the SSL connection and releases associated resources.
     * @param ssl The SSL structure representing the connection.
     */
    void closeSSLConnection(SSL* ssl);

    /**
     * @brief Sends data over an SSL connection.
     * @param ssl The SSL structure representing the connection.
     * @param data The data to be sent.
     * @return The number of bytes sent.
     */
    int sendData(SSL* ssl, const std::string& data);

    /**
     * @brief Receives data from an SSL connection.
     * @param ssl The SSL structure representing the connection.
     * @param buffer The string to store received data.
     * @return The number of bytes received.
     */
    int receiveData(SSL* ssl, std::string& buffer);

    /**
     * @brief Loads a certificate file for SSL verification.
     * @param certFile Path to the certificate file.
     * @throws Terminates the program if the certificate cannot be loaded.
     */
    void setCertificate(const std::string& certFile);

    /**
     * @brief Sets the directory containing certificates for SSL verification.
     * @param certDir Path to the certificate directory.
     * @throws Terminates the program if the directory cannot be loaded.
     */
    void setCertDirectory(const std::string& certDir);

private:
    SSL_CTX* ctx;

    SSLWrapper();
    ~SSLWrapper();

    SSLWrapper(const SSLWrapper&) = delete;
    SSLWrapper& operator=(const SSLWrapper&) = delete;

    /**
     * @brief Initializes the SSL context using TLS client method.
     * @throws Terminates the program if context creation fails.
     */
    void initContext();
};

#endif //IMAP_TLS_CLIENT_SSLWRAPPER_H
