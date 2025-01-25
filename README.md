# SocketFileTransfer-C

A client-server application for secure file transfer with authentication, implemented using TCP sockets in C. The server validates user credentials and handles file uploads/downloads, while the client interacts with the server to perform these operations.

## Features

- **Authentication**: Validates username and password (default: `admin`/`password123`).
- **File Upload**: Clients can send files to the server (saved as `uploaded_<filename>`).
- **File Download**: Clients can request files from the server (saved as `received_<filename>`).
- **Error Handling**: Basic error checks for invalid credentials, missing files, and connection issues.

## Prerequisites

- `gcc` compiler
- Unix-like environment (tested on Linux)
- Basic C libraries (`libc`, `pthread`, etc.)

## Usage

### Compilation

Compile the server and client programs:

```bash
gcc socket_server.c -o server
gcc socket_client.c -o client
