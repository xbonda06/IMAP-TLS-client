cmake_minimum_required(VERSION 3.22)
project(imapcl)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenSSL REQUIRED)

include_directories(include)

add_executable(imapcl src/main.cpp
        src/ArgParser.cpp
        src/IMAPClient.cpp
        src/SSLWrapper.cpp
)

target_link_libraries(imapcl PRIVATE OpenSSL::SSL OpenSSL::Crypto)
