// server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 2120
#define BUFFER_SIZE 1024
#define VALID_USER "admin"
#define VALID_PASS "password123"

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Erreur création socket");
        exit(1);
    }
    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Erreur bind");
        exit(1);
    }
    
    if (listen(server_fd, 5) < 0) {
        perror("Erreur listen");
        exit(1);
    }
    
    printf("Serveur en écoute sur le port %d...\n", PORT);
    
    while (1) {
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("Erreur accept");
            continue;
        }
        
        printf("Client connecté: %s\n", inet_ntoa(client_addr.sin_addr));
        
        // Authentication
        memset(buffer, 0, BUFFER_SIZE);
        recv(client_fd, buffer, BUFFER_SIZE, 0);
        if (strcmp(buffer, VALID_USER) != 0) {
            send(client_fd, "NOK", 3, 0);
            close(client_fd);
            continue;
        }
        
        memset(buffer, 0, BUFFER_SIZE);
        recv(client_fd, buffer, BUFFER_SIZE, 0);
        if (strcmp(buffer, VALID_PASS) != 0) {
            send(client_fd, "NOK", 3, 0);
            close(client_fd);
            continue;
        }
        
        send(client_fd, "OK", 2, 0);
        
        // Receive operation type (UPLOAD/DOWNLOAD)
        memset(buffer, 0, BUFFER_SIZE);
        recv(client_fd, buffer, BUFFER_SIZE, 0);
        
        if (strcmp(buffer, "DOWNLOAD") == 0) {
            // Handle download
            memset(buffer, 0, BUFFER_SIZE);
            recv(client_fd, buffer, BUFFER_SIZE, 0);
            
            FILE *file = fopen(buffer, "r");
            if (file == NULL) {
                send(client_fd, "ERR", 3, 0);
                close(client_fd);
                continue;
            }
            
            send(client_fd, "OK", 2, 0);
            
            size_t bytes_read;
            while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
                send(client_fd, buffer, bytes_read, 0);
            }
            
            send(client_fd, "EOF", 3, 0);
            fclose(file);
        }
        else if (strcmp(buffer, "UPLOAD") == 0) {
            // Handle upload
            memset(buffer, 0, BUFFER_SIZE);
            recv(client_fd, buffer, BUFFER_SIZE, 0);
            
            char filename[150];
            snprintf(filename, sizeof(filename), "uploaded_%s", buffer);
            FILE *file = fopen(filename, "wb");
            
            if (file == NULL) {
                send(client_fd, "ERR", 3, 0);
                close(client_fd);
                continue;
            }
            
            send(client_fd, "OK", 2, 0);
            
            // Receive file content
            while (1) {
                memset(buffer, 0, BUFFER_SIZE);
                int bytes_received = recv(client_fd, buffer, BUFFER_SIZE, 0);
                if (bytes_received <= 0) break;
                
                if (bytes_received == 3 && strcmp(buffer, "EOF") == 0) break;
                
                fwrite(buffer, 1, bytes_received, file);
            }
            
            fclose(file);
            printf("Fichier reçu et sauvegardé: %s\n", filename);
        }
        
        close(client_fd);
    }
    
    close(server_fd);
    return 0;
}