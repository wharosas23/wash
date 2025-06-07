#include <stdio.h>
#include <string.h>

#define MAX_INPUT 256

// Function that prints out help information
void print_help(){
    printf("\nWelcome to the Wash shell! Here are some commands you can use:\n");
    printf("    exit        - Exit the shell (NOT READY)\n");
    printf("    echo        - Print arguments to the console (NOT READY)\n");
    printf("    pwd         - Print the current working directory (NOT READY)\n");
    printf("    cd          - Change the current directory (NOT READY)\n");
    printf("    setpath     - Set the path to look for executables (NOT READY)\n");
    printf("    help        - Show the help message\n\n");
}

void commandInput(int argc, char *argv[]) {
    
    if (strncmp(argv[0], "echo", 4) == 0) {
        printf("You typed: %s\n", argv[1]);
    } else if (strncmp(argv[0], "pwd", 3) == 0) {
        printf("Current working directory command is not implemented yet.\n");
    } else if (strncmp(argv[0], "cd", 2) == 0) {
        printf("Change directory command is not implemented yet.\n");
    } else if (strncmp(argv[0], "setpath", 7) == 0) {
        printf("Set path command is not implemented yet.\n");
    } else if (strncmp(argv[0], "help", 4) == 0) {
        print_help();
    } else {
        printf("%s  command not found\n", argv[0]);
    }

}

// Main function that handles the shell loop
int main(int argc, char *argv[]) {

    // If the user provided the -h flag in the command-line argument,
    // the program prints out the help message and exits 
    if (argc > 1) {

        if (strcmp(argv[1], "-h") == 0) {
            print_help();
            return 0;
        }
    }

    // Begin shell loop
    char input[MAX_INPUT];

    while (1) {
        
        printf("wash> ");
        fflush(stdout);

        fgets(input, MAX_INPUT, stdin);

        input[strcspn(input, "\n")] = '\0';

        // If the user provided no input, then the program outputs a message and exits
        if (strcmp(input, "exit") == 0) {
            printf("\nGoodbye!\n\n");
            return 0;
        }

        // Tokenize input into arguments
        char *args[MAX_INPUT / 2 + 1]; // Enough for simple tokenization
        int arg_count = 0;
        char *token = strtok(input, " ");
        while (token != NULL && arg_count < (MAX_INPUT / 2)) {
            args[arg_count++] = token;
            token = strtok(NULL, " ");
        }
        args[arg_count] = NULL;

        if (arg_count > 0) {
            commandInput(arg_count, args);
        }
    }

    return 0;
}
