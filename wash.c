/**
 * Title: My Own Shell (Extra Credit)
 * Author: Wendy Arenas Rosas
 * CSC 3350, Andy Cameron
 * Due Date: 2025-06-07
 * Description: 
 * This program implements a simple shell called "wash" that can execute built-in commands such as exit, echo, pwd, cd, and help.
 * Additionally, it provides output redirection functionality using the '>' operator. 
 * **/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_INPUT 256

/** This function prints out the help message for the wash shell, letting the user know what commands they can use.
 * The help message is printed when the user runs the command "help" or when they run the program with the "-h" flag.
 * Note: The message does not display setpath command as I did not have time to implement it.
 * **/
void print_help() {

    printf("\nWelcome to the Wash shell! Here are some commands you can use:\n");
    printf("    exit        - Exit the shell (NOT READY)\n");
    printf("    echo        - Print arguments to the console (NOT READY)\n");
    printf("    pwd         - Print the current working directory (NOT READY)\n");
    printf("    cd          - Change the current directory (NOT READY)\n");
    printf("    help        - Show the help message\n\n");
}

/** Function that process the user commands. It handles the commands echo, pwd, cd, and help. 
 * It takes in the argument count (argc) and argument vector (argv) from the main function.
 * argc is the number of arguments, and argv is an array of strings containing the arguments.
 * If the user inputs an invalid command, it prints out a message saying that the command was not found.
 **/
void commandInput(int argc, char *argv[]){

    // Case 1: the user inputs the command "echo"
    if (strcmp(argv[0], "echo") == 0) {

        // If the user did provide the argument "echo" (which would be saved in argv[0]), it iterates through 
        // the rest of the arguments in argv and prints out each argument separated by a space. This is done incase
        // the user inputs multiple arguments after "echo" (such as a sentence).

        for (int i = 1; i < argc; i++){
            printf("%s ", argv[i]);
        }

        printf("\n");
    }
    // Case 2: the user inputs the command "pwd"
    else if (strcmp(argv[0], "pwd") == 0) {

        // First, check if the user provided any arguments besides "pwd". If so, print a error message and return.
        if (argc > 1) {
            fprintf(stderr, "wash: pwd does not take any arguments\n");
            return;
        }

    // The variable cwd will store the path of the current working directory.
    // The getcwd function retrieves the current working directory and stores it in cwd.
    // If successful, it prints the current working directory; otherwise, it prints an error message.
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL){
            printf("%s\n", cwd);
        } 
        else{
            perror("getcwd error");
        }
    }

    // Case 3: the user inputs the command "cd"
    else if (strcmp(argv[0], "cd") == 0){
        
        // The variable target_dir will store the path of the directory that the user wants to change to.
        const char *target_dir;

        // If no arguments are provided by the user, change to the user’s home directory
        if (argc == 1){
            target_dir = getenv("HOME");

            // If the HOME environment variable is not set, print an error message and return
            if (target_dir == NULL){
                fprintf(stderr, "wash: HOME environment not set\n");
                return;
            }
        }
        // If an more than one argument is provided, print an error message and return
        else if (argc > 2) {
            fprintf(stderr, "Too many arguments for cd\n");
            return;
        }
        
        // If a single argument is provided, set target_dir to that argument
        else {
            target_dir = argv[1];
        }

        // Now check if the target directory exists and change to it, otherwise print an error message indicating
        // that the directory was not found.
        if (chdir(target_dir) != 0){
            fprintf(stderr, "%s not found\n", target_dir);
        }
    }

    // Case 4: the user inputs the command "help"
    else if (strcmp(argv[0], "help") == 0) {
        
        // If more than one argument was provided by the user, print an error message and return.
        // Other wise, print the help message.
        if (argc > 1) {
        fprintf(stderr, "wash: help does not take any arguments\n");
        return;
    }
        print_help();
    }

    // If the user inputs any other command, print a message saying that the command was not found.
    else{
        printf("%s command not found\n", argv[0]);
    }

}

/** Function that handles output redirection for commands. 
 * It takes the user input, a pointer to the filename, and pointers to the original stdout and stderr file streams.
 *  It returns 1 on success and 0 on failure. The variable filename will store the name of the file to which output is redirected. **/
int redirection(char *input, char **filename, FILE **stdout_backup, FILE **stderr_backup) {

    // First, the function looks for the '>' character in the input string, which indicates output redirection.
    char *redir_ptr = strchr(input, '>');

    // If '>' is found, it splits the input into the command part and the filename part.
    if (redir_ptr != NULL) {

        //It first terminates the command string by replacing the > with a null character. Thenit moves the poointer past the '>' character, so it points at the space before the filename.
        *redir_ptr = '\0';
        redir_ptr++;

        //Then it skips any spaces after the '>' character to find the actual filename. Once it finds the filename, redir_pot should correctly point to the start of the filename.
        while (*redir_ptr == ' ') redir_ptr++; 

        // If there was not file name provided after the '>', it prints an error message and returns 0.
        if (*redir_ptr == '\0') {
            fprintf(stderr, "Error: no filename provided\n");
            return 0;
        }

        // Otherwise, it sets the filename pointer to the redirection pointer, which now points to the filename.
        *filename = redir_ptr;
        
        // The variable output_file and error_file will store the names of the output and error files. Both files will have the same name as the filename provided by the user, but with ".output" and ".error" appended to them.
        char output_file[512], error_file[512];
        
        snprintf(output_file, sizeof(output_file), "%s.output", *filename);
        snprintf(error_file, sizeof(error_file), "%s.error", *filename);

        // These lines back up the current stdout and stderr streams.
        // `fileno(stdout)` and `fileno(stderr)` get the file descriptors for stdout and stderr.
        // `dup()` creates a duplicate of each file descriptor.
        // `fdopen()` then converts those descriptors back into FILE* streams.
        // This backup allows us to safely restore the original output streams after redirection is complete.
        *stdout_backup = fdopen(dup(fileno(stdout)), "w");
        *stderr_backup = fdopen(dup(fileno(stderr)), "w");

        // Now, it attempts to redirect stdout and stderr to the specified files.
        // If it fails to open either file, it prints an error message and returns 0.
        if (!freopen(output_file, "w", stdout) || !freopen(error_file, "w", stderr)) {
            fprintf(stderr, "wash: failed to redirect output\n");
            return 0;
        }
    }

    return 1;
}

/** The main function will handle the shell loop, as well as the -h flag if provided by the user **/
int main(int argc, char *argv[]){

    // If the user provided the -h flag in the command-line argument,
    // the program prints out the help message and exits
    if (argc > 1){
        if (strcmp(argv[1], "-h") == 0){
            print_help();
            return 0;
        }
    }
 
    // If no flags were provided, the shell loop will begin. It will only stop when the user types "exit" on the command line.
    // The variable input is declared, which will store the user input.
    char input[MAX_INPUT];

    while (1){

        // To replicate a shell prompt, the program prints "wash> " to the console after each command is executed.
        printf("wash> ");
        fflush(stdout);

        // The program reads the user input from the console and stores it in the input variable.
        fgets(input, MAX_INPUT, stdin);

        // Remove the newline character from the input string
        // This is done to ensure that the input string does not contain a newline character at the end, 
        // which would interfere with command processing.
        input[strcspn(input, "\n")] = '\0';

        // If the user provided the command "exit", the program will print a goodbye message and exit the loop, 
        // thus terminating the shell.
        if (strcmp(input, "exit") == 0){
            printf("\nGoodbye!\n\n");
            return 0;
        }

        // First the program will check if the user provided the redcirection operator '>' in their input.
        // It does so by calleing the redirection function, which will handle the redirection of 
        // stdout and stderr to a file if specified.
        // If the user did not provide the redirection operator '>', the filename will be NULL, and the program will not redirect stdout and stderr.
        char *filename = NULL;
        FILE *stdout_backup = NULL;
        FILE *stderr_backup = NULL;

        if (!redirection(input, &filename, &stdout_backup, &stderr_backup)) {
            continue;
        }

        // This block tokenizes the user's input into individual arguments.
        // It uses strtok() to split the input string by spaces, storing each token in the args array.
        // The variable arg_count keeps track of how many arguments were found.
        // args will be passed to the command handler to identify which built-in to execute.

        char *args[MAX_INPUT / 2 + 1];
        int arg_count = 0;
        char *token = strtok(input, " ");

        while (token != NULL && arg_count < (MAX_INPUT / 2)) {
            args[arg_count++] = token;
            token = strtok(NULL, " ");
        }

        args[arg_count] = NULL;

        // Given a possible command, the program will now call the commandInput function to process the command.
        // If the user provided the redirection operator '>', the commandInput function will execute the command 
        // and redirect stdout and stderr to the specified file.
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

    // Once the user exits the shell, the program will return 0 to indicate successful termination.
    return 0;

}