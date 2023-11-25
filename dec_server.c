/*This program performs exactly like enc_server, in syntax and usage. In this case, however, 
dec_server will decrypt ciphertext it is given, using the passed-in ciphertext and key. 
Thus, it returns plaintext again to dec_client.*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

const char* ALLOWED_CHARACTERS = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

// Error function used for reporting issues
void error(const char *msg) {
  perror(msg);
  exit(1);
} 

// Set up the address struct for the server socket
void setupAddressStruct(struct sockaddr_in* address, 
                        int portNumber){
 
  // Clear out the address struct
  memset((char*) address, '\0', sizeof(*address)); 

  // The address should be network capable
  address->sin_family = AF_INET;
  // Store the port number
  address->sin_port = htons(portNumber);
  // Allow a client at any address to connect to this server
  address->sin_addr.s_addr = INADDR_ANY;
}

// Converts a character to its corresponding integer value
int charToInt(char c) {
  if (c >= 'A' && c <= 'Z') {
      return c - 'A';
  } else if (c == ' ') {
      return 26;
  } else {
      // Handle invalid characters
      return -1;
  }
}

// Converts an integer to its corresponding character value
char intToChar(int i) {
  // If the integer is negative, add 27 to make it positive
  if (i < 0) {
      i += 27;
  }
  // If the integer is greater than 26, subtract 27 to wrap it around
  else if (i > 26) {
      i -= 27;
  }
  // Return the character at the corresponding index in the ALLOWED_CHARACTERS array
  return ALLOWED_CHARACTERS[i];
}

int main(int argc, char *argv[]){
  int connectionSocket, charsRead;
  char buffer[256];
  struct sockaddr_in serverAddress, clientAddress;
  socklen_t sizeOfClientInfo = sizeof(clientAddress);

  // Check usage & args
  if (argc < 2) { 
    fprintf(stderr,"USAGE: %s port\n", argv[0]); 
    exit(1);
  } 
  
  // Create the socket that will listen for connections
  int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (listenSocket < 0) {
    error("ERROR opening socket");
  }

  // Set up the address struct for the server socket
  setupAddressStruct(&serverAddress, atoi(argv[1]));

  // Associate the socket to the port
  if (bind(listenSocket, 
          (struct sockaddr *)&serverAddress, 
          sizeof(serverAddress)) < 0){
    error("ERROR on binding");
  }

  // Start listening for connetions. Allow up to 5 connections to queue up
  listen(listenSocket, 5); 
  
  // Accept a connection, blocking if one is not available until one connects
  while(1){
    // Accept the connection request which creates a connection socket
    connectionSocket = accept(listenSocket, 
                (struct sockaddr *)&clientAddress, 
                &sizeOfClientInfo); 
    if (connectionSocket < 0){
      error("ERROR on accept");
    }

    printf("SERVER: Connected to client running at host %d port %d\n", 
                          ntohs(clientAddress.sin_addr.s_addr),
                          ntohs(clientAddress.sin_port));

    // Get the message from the client and display it
    memset(buffer, '\0', 256);

    char ciphertext[256]; // holds the result of final decrypted ciphertext to send back to client

    // Read the client's message from the socket and map them to their integer values
    while ((charsRead = recv(connectionSocket, buffer, sizeof(buffer) - 1, 0)) > 0) {
        printf("SERVER: I received this from the client: \"%s\"\n", buffer);
        memset(ciphertext, '\0', sizeof(ciphertext));  // Clear the ciphertext string
        for (int i = 0; i < charsRead; i += 2) {
            char plaintextChar = buffer[i];
            char keyChar = buffer[i+1];
            // Convert plaintext char and key char to their integer representations
            int decryptedChar1 = charToInt(plaintextChar);
            int decryptedChar2 = charToInt(keyChar);
            // Subtract the key from the ciphertext and add 27 to handle negative results
            int diff = (decryptedChar1 - decryptedChar2 + 27) % 27;
            char decryptedChar = intToChar(diff);  // Convert the integer to its corresponding character
            strncat(ciphertext, &decryptedChar, 1);  // Append the encrypted character to the ciphertext string
        }
        send(connectionSocket, ciphertext, strlen(ciphertext), 0);  // Send the decoded ciphertext string to the client
    }
    // Close the connection socket for this client
    close(connectionSocket); 
  }
  // Close the listening socket
  close(listenSocket); 
  return 0;
}
