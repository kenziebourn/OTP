/*This program creates a key file of specified length. The characters in the file generated will be 
any of the 27 allowed characters, generated using the standard Unix randomization methods.*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ALLOWED_CHARACTERS "ABCDEFGHIJKLMNOPQRSTUVWXYZ "

int main(int argc, char *argv[]) 
{
    // Check the command-line arguments
    if (argc != 2) {
        fprintf(stderr, "Too many args!\n");
        return 1;
    }

    // Get the key length from the command-line argument
    int keyLength = atoi(argv[1]);

    // Seed the random number generator
    srand(time(NULL));

    // Generate the key
    for (int i = 0; i < keyLength; i++) {
        int randomIndex = rand() % 27;
        char randomChar = ALLOWED_CHARACTERS[randomIndex];
        printf("%c", randomChar);
    }
    // Print a newline at the end
    printf("\n");
    return 0;
}