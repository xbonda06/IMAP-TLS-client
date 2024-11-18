# IMAP-TLS-client
3BIT project (VUT FIT). The imapcl program, which allows reading electronic mail using the IMAP4rev1 protocol (RFC 3501).

# IMAP Client (`imapcl`)

## Author
- **Name**: Ivan Ivanov
- **Login**: xbonda06
- **Date**: 18.11.2024

## Description
This is the 3BIT project (VUT FIT). The imapcl program, which allows reading electronic mail using the IMAP4rev1 protocol (RFC 3501) with support for SSL/TLS.

## Usage
```bash
imapcl <server> [-p port] [-T [-c certfile] [-C certdir]] [-n] [-h] -a auth_file -o out_dir
```

### Options
- `<server>`: The domain or IP address of the IMAP server.
- `-p port`: Specify the server port (default: 143 for non-TLS, 993 for TLS).
- `-T`: Enable SSL/TLS encryption.
- `-c certfile`: Specify a certificate file to use for SSL/TLS verification.
- `-C certdir`: Specify a directory containing certificates for SSL/TLS verification (default: `/etc/ssl/certs`).
- `-n`: Download only new messages.
- `-h`: Download only message headers.
- `-a auth_file`: Path to the file containing authentication credentials.
- `-o out_dir`: Output directory where emails will be saved.

## Examples
### 1. Connecting to a server without SSL
```bash
imapcl mail.example.com -o maildir -a cred
```

### 2. Connecting to a server with SSL
```bash
imapcl 10.10.10.1 -p 993 -T -c cert.pem -C /etc/ssl/certs -a cred -o maildir
```

## Project Structure
```
├── include
│   ├── ArgParser.h
│   ├── ConnectionStrategy.h
│   ├── FetchByIdCommand.h
│   ├── FetchCommand.h
│   ├── IMAPClient.h
│   ├── IMAPCommand.h
│   ├── IMAPCommandFactory.h
│   ├── IMAPExceptions.h
│   ├── IMAPResponceType.h
│   ├── LoginCommand.h
│   ├── LogoutCommand.h
│   ├── SearchCommand.h
│   ├── SelectCommand.h
│   ├── SSLConnectionStrategy.h
│   ├── SSLWrapper.h
│   ├── TCPConnectionStrategy.h
├── src
│   ├── ArgParser.cpp
│   ├── IMAPClient.cpp
│   ├── SSLWrapper.cpp
│   ├── ConnectionStrategy.cpp
│   ├── SSLConnectionStrategy.cpp
│   ├── TCPConnectionStrategy.cpp
│   ├── main.cpp
├── Makefile
├── CMakeLists.txt
├── README.md
├── LICENSE
└── manual.pdf

```

## Compilation

To compile the project, use the provided Makefile:
```bash
make
```

To clean up the build:
```bash
make clean
```

After compiling, the executable will be named `imapcl`.

## Notes
- The application supports both encrypted (SSL/TLS) and unencrypted IMAP connections.
- Ensure that the OpenSSL library is installed on your system.
- The application does not produce segmentation faults or crashes under normal usage.
- Any limitations or unimplemented features are documented in `manual.pdf`.