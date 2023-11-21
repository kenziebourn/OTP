# OTP

This project implements a one-time pad encryption system using multiple programs and inter-process communication. It is designed to apply concepts from my operating systems and computer networks courses.

Five programs are created to encrypt plaintext, decrypt ciphertext, and facilitate communication between the processes. Multiprocessing allows encryption on one machine and decryption on another. Socket programming provides client-server functionality for data exchange across a network.

Programs are invoked from the command line, making use of Unix I/O redirection and job control. One program acts as a server while others connect as clients, allowing for asynchronous encryption and decryption workflows.

A one-time pad cipher is applied using randomly generated keys to convert plaintext to unbreakable ciphertext securely. Proper key usage and disposal is important for security.

Inter-process communication via sockets allows the programs to work independently while still collaborating on the encryption task. A shell script ties it all together by automating common encryption/decryption workflows. The projects aims to strengthen my understanding of core operating systems and networking topics.
