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
```

### Running the Server

1. Start the server first:
   ```bash
   ./server
   ```
2. The server listens on port `2120` (ensure the port is available and not blocked by a firewall).

### Running the Client

1. Run the client and connect to the server:
   ```bash
   ./client
   ```
2. Enter credentials:
   - Username: `admin`
   - Password: `password123`
3. Choose an operation:
   - **Download**: Request a file from the server.
   - **Upload**: Send a file to the server.

### Notes

- Update `SERVER_IP` in `socket_client.c` to match your server's IP address before compiling.
- The server handles one client at a time (single-threaded).

## Code Structure

- `socket_server.c`: Server-side code for authentication and file management.
- `socket_client.c`: Client-side code for user interaction and file transfer.

