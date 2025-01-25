//client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 2120
#define BUFFER_SIZE 1024
#define SERVER_IP "192.168.43.37"

int main() {
    int sock_fd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    char username[50];
    char password[50];
    char filename[100];
    int choice;
    FILE *file;
    
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("Erreur création socket");
        exit(1);
    }
    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Adresse invalide");
        exit(1);
    }
    
    if (connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Erreur connexion");
        exit(1);
    }
    
    printf("Username: ");
    fgets(username, 50, stdin);
    username[strcspn(username, "\n")] = 0;
    send(sock_fd, username, strlen(username), 0);
    
    printf("Password: ");
    fgets(password, 50, stdin);
    password[strcspn(password, "\n")] = 0;
    send(sock_fd, password, strlen(password), 0);
    
    memset(buffer, 0, BUFFER_SIZE);
    recv(sock_fd, buffer, BUFFER_SIZE, 0);
    if (strcmp(buffer, "NOK") == 0) {
        printf("Authentification échouée\n");
        close(sock_fd);
        exit(1);
    }
    
    printf("\nChoisissez une opération:\n");
    printf("1. Download un fichier\n");
    printf("2. Upload un fichier\n");
    printf("Choix (1 ou 2): ");
    scanf("%d", &choice);
    getchar(); // Pour consommer le \n restant
    
    if (choice == 1) {
        send(sock_fd, "DOWNLOAD", 8, 0);
        
        printf("Nom du fichier à télécharger: ");
        fgets(filename, 100, stdin);
        filename[strcspn(filename, "\n")] = 0;
        send(sock_fd, filename, strlen(filename), 0);
        
        memset(buffer, 0, BUFFER_SIZE);
        recv(sock_fd, buffer, BUFFER_SIZE, 0);
        if (strcmp(buffer, "ERR") == 0) {
            printf("Erreur: fichier non trouvé\n");
            close(sock_fd);
            exit(1);
        }
        
        char output_filename[150];
        snprintf(output_filename, sizeof(output_filename), "received_%s", filename);
        file = fopen(output_filename, "wb");
        if (file == NULL) {
            perror("Erreur création fichier");
            close(sock_fd);
            exit(1);
        }
        
        printf("Réception du fichier...\n");
        while (1) {
            memset(buffer, 0, BUFFER_SIZE);
            int bytes_received = recv(sock_fd, buffer, BUFFER_SIZE, 0);
            if (bytes_received <= 0) break;
            
            if (bytes_received == 3 && strcmp(buffer, "EOF") == 0) break;
            
            fwrite(buffer, 1, bytes_received, file);
        }
        
        fclose(file);
        printf("Fichier reçu et sauvegardé sous: %s\n", output_filename);
    }
    else if (choice == 2) {
        send(sock_fd, "UPLOAD", 6, 0);
        
        printf("Nom du fichier à uploader: ");
        fgets(filename, 100, stdin);
        filename[strcspn(filename, "\n")] = 0;
        
        file = fopen(filename, "rb");
        if (file == NULL) {
            printf("Erreur: impossible d'ouvrir le fichier\n");
            close(sock_fd);
            exit(1);
        }
        
        // Send filename to server
        send(sock_fd, filename, strlen(filename), 0);
        
        memset(buffer, 0, BUFFER_SIZE);
        recv(sock_fd, buffer, BUFFER_SIZE, 0);
        if (strcmp(buffer, "ERR") == 0) {
            printf("Erreur: le serveur ne peut pas créer le fichier\n");
            fclose(file);
            close(sock_fd);
            exit(1);
        }
        
        printf("Envoi du fichier...\n");
        size_t bytes_read;
        while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
            send(sock_fd, buffer, bytes_read, 0);
        }
        
        send(sock_fd, "EOF", 3, 0);
        fclose(file);
        printf("Fichier envoyé avec succès\n");
    }
    else {
        printf("Choix invalide\n");
    }
    
    close(sock_fd);
    return 0;
}