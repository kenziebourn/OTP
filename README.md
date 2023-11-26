# OTP

This project implements a one-time pad encryption system using multiple programs and inter-process communication. It is designed to apply concepts from my operating systems and computer networks courses.

Five programs are created to generate key, encrypt plaintext, decrypt ciphertext, and facilitate communication between the processes. Socket programming provides client-server functionality for data exchange across a network.

Programs are invoked from the command line, making use of Unix I/O redirection and job control. One program acts as a server while others connect as clients, allowing for asynchronous encryption and decryption workflows.

A one-time pad cipher is applied using randomly generated keys to convert plaintext to ciphertext. Inter-process communication via sockets allows the programs to work independently while still collaborating on the encryption/decryption task. A simple Makefile script compiles the five programs. Overall, the projects aims to strengthen my understanding of core operating systems, socket programming and networking topics. 

## To compile
`make keygen enc_client enc_server dec_client dec_server`

## Testing Examples
### Encryption

Startup the encryption server:

`./enc_server 5000 &`

> The enc_server starts running on port 5000 with localhost already predefined in the code.

Encrypt the file with key: 

`./enc_client plaintext key 5000`

> The enc_client reads in a plaintext file and a key file as well as port number 

### Decryption

Startup the decryption server:

`./dec_server 6000 &`

> The dec_server starts running on port 6000 with localhost already predefined in the code

Decrypt a ciphertext file

`./dec_client ciphertext mykey 6000`

> The dec_client reads a ciphertext file and key file as well as port number
