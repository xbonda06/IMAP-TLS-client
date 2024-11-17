cmake_minimum_required(VERSION 3.22)
project(IMAP_TLS_client)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenSSL REQUIRED)

include_directories(include)

add_executable(IMAP_TLS_client src/main.cpp
        src/ArgParser.cpp
        src/IMAPClient.cpp
        src/SSLWrapper.cpp
)

target_link_libraries(IMAP_TLS_client PRIVATE OpenSSL::SSL OpenSSL::Crypto)
