#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_BUFFER_SIZE 1024

typedef struct Node {
    char data[256];
    struct Node* next;
} Node;

Node* readConfigFile(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening configuration file");
        exit(EXIT_FAILURE);
    }

    Node* head = NULL;
    Node* current = NULL;
    char line[256];

    while (fgets(line, sizeof(line), file) != NULL) {
        size_t length = strlen(line);
        if (length > 0 && line[length - 1] == '\n') {
            line[length - 1] = '\0'; // Remove newline character
        }

        Node* newNode = (Node*)malloc(sizeof(Node));
        if (newNode == NULL) {
            perror("Memory allocation error");
            exit(EXIT_FAILURE);
        }

        strcpy(newNode->data, line);
        newNode->next = NULL;

        if (head == NULL) {
            head = newNode;
            current = newNode;
        } else {
            current->next = newNode;
            current = newNode;
        }
    }

    fclose(file);
    return head;
}

void freeLinkedList(Node* head) {
    Node* current = head;
    Node* next;

    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
}



int main() {
    Node* configList = readConfigFile("config.txt");

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(8080);

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        perror("Error binding socket");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    if (listen(serverSocket, 5) == -1) {
        perror("Error listening for connections");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port 8080...\n");

    while (1) {
        int clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket == -1) {
            perror("Error accepting connection");
            close(serverSocket);
            exit(EXIT_FAILURE);
        }

        char buffer[MAX_BUFFER_SIZE];
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0); 
        if (bytesRead == -1) {
            perror("Error receiving data");
            close(clientSocket);
            continue;
        }

        buffer[bytesRead] = '\0'; 

        // Remove newline character if present
        size_t length = strlen(buffer);
        if (length > 0 && buffer[length - 1] == '\n') {
            buffer[length - 1] = '\0';
        }

        int match = 0;
        Node* current = configList;
        while (current != NULL) {
            printf("Comparing '%s' with '%s'\n", buffer, current->data);
            if (strcmp(buffer, current->data) == 0) {
                match = 1;
                break;
            }
            current = current->next;
        }

        const char* response = (match) ? "OK" : "Failed";
        send(clientSocket, response, strlen(response), 0);

        close(clientSocket);
    }

    freeLinkedList(configList);
    close(serverSocket);

    return 0;
}
