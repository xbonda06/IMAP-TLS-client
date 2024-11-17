CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
LDFLAGS = -lssl -lcrypto
SRC = src/ArgParser.cpp src/IMAPClient.cpp src/SSLWrapper.cpp src/main.cpp
INC = -Iinclude
TARGET = imapcl

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) $(INC) $(LDFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all clean
