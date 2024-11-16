cmake_minimum_required(VERSION 3.22)
project(IMAP_TLS_client)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable(IMAP_TLS_client src/main.cpp
        src/ArgParser.cpp
        src/IMAPClient.cpp
        include/LogoutCommand.h
)
