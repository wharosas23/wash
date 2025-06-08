#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_INPUT 256

// Function that prints out help information
void print_help()
{
    printf("\nWelcome to the Wash shell! Here are some commands you can use:\n");
    printf("    exit        - Exit the shell (NOT READY)\n");
    printf("    echo        - Print arguments to the console (NOT READY)\n");
    printf("    pwd         - Print the current working directory (NOT READY)\n");
    printf("    cd          - Change the current directory (NOT READY)\n");
    printf("    setpath     - Set the path to look for executables (NOT READY)\n");
    printf("    help        - Show the help message\n\n");
}

void commandInput(int argc, char *argv[])
{

    if (strcmp(argv[0], "echo") == 0)
    {
        for (int i = 1; i < argc; i++)
        {
            printf("%s ", argv[i]);
        }
        printf("\n");
    }
    else if (strcmp(argv[0], "pwd") == 0)
    {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL)
        {
            printf("%s\n", cwd);
        }
        else
        {
            perror("getcwd error");
        }
    }
    else if (strcmp(argv[0], "cd") == 0)
    {
        const char *target_dir;

        if (argc == 1)
        {
            // No argument → use HOME
            target_dir = getenv("HOME");
            if (target_dir == NULL)
            {
                fprintf(stderr, "wash: HOME environment variable not set\n");
                return;
            }
        }
        else
        {
            // Argument provided → use it
            target_dir = argv[1];
        }

        // Try to change directory
        if (chdir(target_dir) != 0)
        {
            fprintf(stderr, "%s not found\n", target_dir);
        }
    }
    else if (strcmp(argv[0], "setpath") == 0)
    {
        printf("Set path command is not implemented yet.\n");
    }
    else if (strcmp(argv[0], "help") == 0)
    {
        print_help();
    }
    else
    {
        printf("%s command not found\n", argv[0]);
    }
}

int handle_redirection(char *input, char **filename, FILE **stdout_backup, FILE **stderr_backup) {
    char *redir_ptr = strchr(input, '>'); // Look for >

    *filename = NULL;

    if (redir_ptr != NULL) {
        *redir_ptr = '\0'; // Terminate command part
        redir_ptr++;

        while (*redir_ptr == ' ') redir_ptr++; // Skip spaces

        if (*redir_ptr == '\0') {
            fprintf(stderr, "wash: missing filename after '>'\n");
            return 0;
        }

        *filename = redir_ptr;

        char output_file[512], error_file[512];
        snprintf(output_file, sizeof(output_file), "%s.output", *filename);
        snprintf(error_file, sizeof(error_file), "%s.error", *filename);

        *stdout_backup = fdopen(dup(fileno(stdout)), "w");
        *stderr_backup = fdopen(dup(fileno(stderr)), "w");

        if (!freopen(output_file, "w", stdout) || !freopen(error_file, "w", stderr)) {
            fprintf(stderr, "wash: failed to redirect output\n");
            return 0;
        }
    }

    return 1;
}

// Main function that handles the shell loop
int main(int argc, char *argv[])
{

    // If the user provided the -h flag in the command-line argument,
    // the program prints out the help message and exits
    if (argc > 1)
    {

        if (strcmp(argv[1], "-h") == 0)
        {
            print_help();
            return 0;
        }
    }

    // Begin shell loop
    char input[MAX_INPUT];

    while (1)
    {

        printf("wash> ");
        fflush(stdout);

        fgets(input, MAX_INPUT, stdin);

        input[strcspn(input, "\n")] = '\0';

        // If the user provided no input, then the program outputs a message and exits
        if (strcmp(input, "exit") == 0)
        {
            printf("\nGoodbye!\n\n");
            return 0;
        }

// Handle redirection (if any)
        char *filename = NULL;
        FILE *stdout_backup = NULL;
        FILE *stderr_backup = NULL;

        if (!handle_redirection(input, &filename, &stdout_backup, &stderr_backup)) {
            continue; // Skip on bad redirection
        }

        // Tokenize the command part of input
        char *args[MAX_INPUT / 2 + 1];
        int arg_count = 0;
        char *token = strtok(input, " ");
        while (token != NULL && arg_count < (MAX_INPUT / 2)) {
            args[arg_count++] = token;
            token = strtok(NULL, " ");
        }
        args[arg_count] = NULL;

        // Execute command
        if (arg_count > 0) {
            commandInput(arg_count, args);

            // Restore original stdout/stderr if redirected
            if (filename != NULL) {
                fflush(stdout);
                fflush(stderr);
                dup2(fileno(stdout_backup), fileno(stdout));
                dup2(fileno(stderr_backup), fileno(stderr));
                fclose(stdout_backup);
                fclose(stderr_backup);
            }
        }
    }

    return 0;

}