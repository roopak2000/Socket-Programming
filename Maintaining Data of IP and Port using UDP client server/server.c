#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAX_CLIENTS 100
#define FILENAME "connections.txt"

struct ClientInfo {
    char ip[INET_ADDRSTRLEN];
    int port;
};

int isDuplicate(struct ClientInfo *clients, int count, struct sockaddr_in client_addr) {
    for (int i = 0; i < count; i++) {
        if (strcmp(clients[i].ip, inet_ntoa(client_addr.sin_addr)) == 0 && clients[i].port == ntohs(client_addr.sin_port)) {
            return 1; // Duplicate entry
        }
    }
    return 0; // Not a duplicate
}

void handleConnection(int sockfd, struct sockaddr_in client_addr, struct ClientInfo *clients, int *clientCount) {
    if (!isDuplicate(clients, *clientCount, client_addr)) {
        FILE *file = fopen(FILENAME, "a");
        if (file != NULL) {
            fprintf(file, "%s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
            fclose(file);

            // Update client information
            strcpy(clients[*clientCount].ip, inet_ntoa(client_addr.sin_addr));
            clients[*clientCount].port = ntohs(client_addr.sin_port);
            (*clientCount)++;

            printf("New connection added: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        } else {
            perror("Error opening file");
        }
    }
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    struct ClientInfo clients[MAX_CLIENTS];
    int clientCount = 0;

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Initialize server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080); // You can change the port as needed

    // Bind socket
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }

    while (1) {
        char buffer[1024];
        int recv_len = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, &addr_len);

        if (recv_len > 0) {
            buffer[recv_len] = '\0';
            printf("Received message: %s\n", buffer);

            // Handle connection and update file
            handleConnection(sockfd, client_addr, clients, &clientCount);
        }
    }

    close(sockfd);

    return 0;
}
