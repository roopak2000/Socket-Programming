#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_BUFFER_SIZE 1024

int main() {
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr);

    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        perror("Error connecting to server");
        close(clientSocket);
        exit(EXIT_FAILURE);
    }

    char dataToSend[MAX_BUFFER_SIZE];  // Change to array to store multiple characters
    printf("Enter data to send: ");
    fgets(dataToSend, MAX_BUFFER_SIZE, stdin);  // Specify buffer size and read from stdin

    send(clientSocket, dataToSend, strlen(dataToSend), 0);

    char buffer[MAX_BUFFER_SIZE];
    ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);  
    if (bytesRead == -1) {
        perror("Error receiving data");
    } else {
        buffer[bytesRead] = '\0';
        printf("Server response: %s\n", buffer);
    }

    close(clientSocket);

    return 0;
}
