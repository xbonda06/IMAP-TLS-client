cmake_minimum_required(VERSION 3.26)
project(IMAP_TLS_client)

set(CMAKE_CXX_STANDARD 17)

add_executable(IMAP_TLS_client src/main.cpp
        src/ArgParser.cpp
        include/ArgParser.h)
