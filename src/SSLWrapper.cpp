//
// Created by Andrii Bondarenko (xbonda06) on 17.11.2024.
//

#include "SSLWrapper.h"
#include <iostream>
#include <cstring>

SSLWrapper::SSLWrapper() : ctx(nullptr) {}

SSLWrapper::~SSLWrapper() {
    cleanupSSL();
}

SSLWrapper& SSLWrapper::getInstance() {
    static SSLWrapper instance;
    return instance;
}

void SSLWrapper::initSSL() {
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    initContext();
}

void SSLWrapper::initContext() {
    const SSL_METHOD* method = TLS_client_method();
    ctx = SSL_CTX_new(method);
    if (!ctx) {
        std::cerr << "Failed to create SSL context" << std::endl;
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
}

void SSLWrapper::setCertificate(const std::string& certFile) {
    if (SSL_CTX_use_certificate_file(ctx, certFile.c_str(), SSL_FILETYPE_PEM) <= 0) {
        std::cerr << "Failed to load certificate file: " << certFile << std::endl;
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
}

void SSLWrapper::setCertDirectory(const std::string& certDir) {
    if (!SSL_CTX_load_verify_locations(ctx, nullptr, certDir.c_str())) {
        std::cerr << "Failed to load certificate directory: " << certDir << std::endl;
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }
}

SSL* SSLWrapper::createSSLConnection(int socket) {
    SSL* ssl = SSL_new(ctx);
    if (!ssl) {
        std::cerr << "Failed to create SSL object" << std::endl;
        return nullptr;
    }

    SSL_set_fd(ssl, socket);
    if (SSL_connect(ssl) <= 0) {
        std::cerr << "SSL connection failed" << std::endl;
        ERR_print_errors_fp(stderr);
        SSL_free(ssl);
        return nullptr;
    }

    return ssl;
}

void SSLWrapper::cleanupSSL() {
    if (ctx) {
        SSL_CTX_free(ctx);
        ctx = nullptr;
    }
}

void SSLWrapper::closeSSLConnection(SSL* ssl) {
    if (ssl) {
        SSL_shutdown(ssl);
        SSL_free(ssl);
    }
}

int SSLWrapper::sendData(SSL* ssl, const std::string& data) {
    return SSL_write(ssl, data.c_str(), data.length());
}

int SSLWrapper::receiveData(SSL* ssl, std::string& buffer) {
    char buf[4096];
    int bytesReceived = SSL_read(ssl, buf, sizeof(buf));
    if (bytesReceived > 0) {
        buffer.append(buf, bytesReceived);
    }
    return bytesReceived;
}
