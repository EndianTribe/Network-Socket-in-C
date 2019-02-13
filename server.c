#include <sys/types.h>                                                                                                      // Required by getaddrinfo(), setsockopt(), bind(), listen(), accept(), recv()
#include <sys/socket.h>                                                                                                     // Required by getaddrinfo(), setsockopt(), bind(), listen(), accept(), recv()
#include <netdb.h>                                                                                                          // Required by getaddrinfo()
#include <string.h>                                                                                                         // Required by memset()
#include <stdio.h>                                                                                                          // Required by perror()
#include <stdlib.h>                                                                                                         // Required by exit()
#include <unistd.h>                                                                                                         // Required by close()

int main() {

    printf("Server started.\n");

    const size_t bufferSize = 1000;                                                                                         // Set global buffer size
    const char *portNumber = "6969";                                                                                        // Set server port number
    const int backlog = 1;                                                                                                  // Set max number of client requests that can remain in queue
    int serverSocket;                                                                                                       // File descriptor for server socket
    struct addrinfo hints;                                                                                                  // Hints provided to getaddrinfo() for translation
    memset(&hints, 0, sizeof(struct addrinfo));                                                                             // Initialize memory for hints
    struct addrinfo *results;                                                                                               // Linked list of results generated by getaddrinfo()
    struct addrinfo *record;                                                                                                // Single record of the linked list pointed by results
    hints.ai_family = AF_INET;                                                                                              // Set IP family to IPv4
    hints.ai_socktype = SOCK_STREAM;                                                                                        // Set socket type to stream
    hints.ai_protocol = IPPROTO_TCP;                                                                                        // Set protocol type to TCP


    if ((getaddrinfo(NULL, portNumber, &hints, &results)) != 0) {                                                           // Translate address
        perror("Failed to translate server socket.");
        exit(EXIT_FAILURE);
    }
    printf("Server socket translated.\n");

    for (record = results; record != NULL; record = record->ai_next) {                                                      // Iterate through every record in results
        serverSocket = socket(record->ai_family, record->ai_socktype, record->ai_protocol);                                 // Attempt to create socket from information provided in current record
        if (serverSocket == -1) continue;                                                                                   // Skip current iteration in the loop if socket creation fails
        int enable = 1;
        setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));                                           // Configure server socket to reuse same port. Helpful if the server needs to restart
        if (bind(serverSocket, record->ai_addr, record->ai_addrlen) == 0) break;                                            // Attempt to bind socket after its successful creation. If successful, break out of the loop
        close(serverSocket);                                                                                                // Close the socket if socket creation is successful but binding is unsuccessful
    }

    if (record == NULL) {                                                                                                   // record will iterate to NULL if the above loop encounters no success
        perror("Failed to create or connect client socket.");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(results);                                                                                                  // results is a linked list and hence dynamically allocated. It must be freed manually

    printf("Server socket created and bound.\n");

    if (listen(serverSocket, backlog) == -1) {                                                                              // Start server socket listen
        perror("Failed to start server socket listen.");
        exit(EXIT_FAILURE);
    }

    printf("Server started listening.\n");

    while (1) {                                                                                                             // Once server socket starts listening, it loops forever

        printf("Server still running.\n");

        int clientSocket;                                                                                                   // File descriptor for client socket
        struct sockaddr clientAddress;                                                                                      // Struct that stores client address information generated by accept()
        socklen_t clientAddressLength = sizeof(clientAddress);

        if ((clientSocket = accept(serverSocket, &clientAddress, &clientAddressLength)) < 0) {                              // Accept client socket
            perror("Failed to accept client socket.");
            exit(EXIT_FAILURE);
        }

        printf("Client socket accepted.\n");

        char buffer[bufferSize];

        if (recv(clientSocket, buffer, sizeof(buffer), 0) == -1) {                                                          // Read message
            perror("Failed to receive message.");
            exit(EXIT_FAILURE);
        }

        printf("---------- RECEIVED MESSAGE ----------\n%s\n----------- END OF MESSAGE -----------\n", buffer);

        close(clientSocket);                                                                                                // Typically it is the client's responsibility to close the client socket. But in this example we close it here as well.

        printf("Client socket closed.\n");
    }

    return 0;
}