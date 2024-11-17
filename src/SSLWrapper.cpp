//
// Created by media on 17.11.2024.
//

#include "SSLWrapper.h"

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

    SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_TLSv1 | SSL_OP_NO_TLSv1_1);

    SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION);
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