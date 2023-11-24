#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>  // ssize_t
#include <sys/socket.h> // send(),recv()
#include <netdb.h>      // gethostbyname()

#define ALLOWED_CHARACTERS "ABCDEFGHIJKLMNOPQRSTUVWXYZ "
/**
* Client code
* 1. Create a socket and connect to the server specified in the command arugments.
* 2. Process plaintext and key files.
* 3. Print the message received from the server and exit the program.
*/

// Error function used for reporting issues
void error(const char *msg) {
    perror(msg); 
    exit(0); 
} 

void setupAddressStruct(struct sockaddr_in* address, int portNumber) {
    // Clear out the address struct
    memset((char*) address, '\0', sizeof(*address)); 

    // The address should be network capable
    address->sin_family = AF_INET;

    // Store the port number
    address->sin_port = htons(portNumber); 

    // Set the IP address to 127.0.0.1
    address->sin_addr.s_addr = inet_addr("127.0.0.1");
}

int main(int argc, char *argv[]) {
    int socketFD, portNumber, charsWritten, charsRead;
    struct sockaddr_in serverAddress;

    // Check usage & args
    if (argc < 3) { 
        fprintf(stderr,"USAGE: %s port\n", argv[0]); 
        exit(0); 
    } 

    // Create a socket
    socketFD = socket(AF_INET, SOCK_STREAM, 0); 
    if (socketFD < 0){
        error("CLIENT: ERROR opening socket");
    }

    // Set up the server address struct with given port number
    setupAddressStruct(&serverAddress, atoi(argv[3]));

    // Connect to server
    if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){
        error("CLIENT: ERROR connecting");
    }
    // Process plaintext file
    FILE *plaintextFile = fopen(argv[1], "r");
    if (plaintextFile == NULL) {
        fprintf(stderr, "Error opening plaintext file\n");
        exit(1);
    }

    // Get the size of the plaintext
    fseek(plaintextFile, 0, SEEK_END);
    long plaintextSize = ftell(plaintextFile);
    fseek(plaintextFile, 0, SEEK_SET);

    // Allocate memory for the plaintext
    char *plaintext = malloc(plaintextSize + 1);
    if (plaintext == NULL) {
        fprintf(stderr, "Error allocating memory for plaintext\n");
        exit(1);
    }

    // Read the plaintext into the buffer
    fread(plaintext, 1, plaintextSize, plaintextFile);
    plaintext[strcspn(plaintext, "\n")] = '\0'; // strip off the newline
    plaintext[plaintextSize] = '\0'; // Null-terminate the string
    fclose(plaintextFile);

    printf("CLIENT: plaintext: %s\n",plaintext); // debugging

    // Process key file
    FILE *keyFile = fopen(argv[2], "r");
    if (keyFile == NULL) {
        fprintf(stderr, "Error opening key file\n");
        exit(1);
    }
    // Get size of key 
    fseek(keyFile, 0, SEEK_END);
    long keySize = ftell(keyFile);
    fseek(keyFile, 0, SEEK_SET);

    // Allocate memory for key
    char *key = malloc(keySize + 1);
    if (key == NULL) {
        fprintf(stderr, "Error allocating memory for key\n");
        exit(1);
    }

    // Read the key into the buffer
    fread(key, 1, keySize, keyFile);
    key[keySize] = '\0'; // Null-terminate the string
    fclose(keyFile);

    //printf("CLIENT: key size: %ld\n",keySize); // debugging
    //printf("CLIENT: plaintext size: %ld\n",plaintextSize); // debugging

    // Check key length
    if (keySize < plaintextSize) {
        fprintf(stderr, "Error: key '%s' is too short\n", key);
        exit(1);
    }

    // Check for bad characters
    int i;
    for (i = 0; i < plaintextSize; i++) {
        if (strchr(ALLOWED_CHARACTERS, plaintext[i]) == NULL) {
            fprintf(stderr, "Error: input contains bad characters\n");
            exit(1);
        }
    }

    // Send 1 char at a time from plaintext to server
    char buffer[3]; // Buffer to hold two characters and a null terminator
    for (i = 0; i < plaintextSize; i += 1) {
        // Check if we reached end of plaintext
        if (plaintext[i] == '\0') {
            break;
        }
        // If plaintext has space character, send it to server still
        if (plaintext[i] == ' ') {
            buffer[0] = ' ';
            buffer[1] = key[i];
            buffer[2] = '\0';
            charsWritten = send(socketFD, buffer, strlen(buffer), 0);
        }
        else {
            // Copy two characters from the plaintext to the buffer
            strncpy(buffer, &plaintext[i], 1);
            strncpy(&buffer[1], &key[i], 1);
            buffer[2] = '\0'; // Null-terminate the string

            /*Debugging*/
            printf("CLIENT: buffer chars: %s\n",buffer);
            // Send the buffer to the server
            charsWritten = send(socketFD, buffer, strlen(buffer), 0); 
            if (charsWritten < 0){
                error("CLIENT: ERROR writing to socket");
            }
            if (charsWritten < strlen(buffer)){
                printf("CLIENT: WARNING: Not all data written to socket!\n");
            }
        }
        // Get return message from server
        // Clear out the buffer again for reuse
        memset(buffer, '\0', sizeof(buffer));
        // Read data from the socket, leaving \0 at end
        charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); 
        if (charsRead < 0){
            error("CLIENT: ERROR reading from socket");
        }
        printf("CLIENT: I received this from the server: \"%s\"\n", buffer);
    }

    // Close the socket
    close(socketFD); 
    return 0;
}