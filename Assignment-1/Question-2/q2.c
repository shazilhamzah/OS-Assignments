#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_INPUT_LENGTH 1024
#define MAX_TOKENS 64

int main()
{
    char input[MAX_INPUT_LENGTH];
    char input_copy[MAX_INPUT_LENGTH];
    char *tokens[MAX_TOKENS];
    char *token;
    int i;
    const char *exit_cmd = "exit";

    while (1)
    {
        printf("Enter Command: ");
        fflush(stdout);

        if (fgets(input, MAX_INPUT_LENGTH, stdin) == NULL)
        {
            break; // Handle EOF (Ctrl+D)
        }

        input[strcspn(input, "\n")] = '\0'; // Remove newline

        if (strcmp(input, exit_cmd) == 0)
        {
            printf("Goodbye, challenger! See you in the next duel.\n");
            break;
        }

        strcpy(input_copy, input); // Save a copy of the original input for display

        // Tokenize the input
        i = 0;
        token = strtok(input, " \t");
        while (token != NULL && i < MAX_TOKENS - 1)
        {
            tokens[i] = token;
            i++;
            token = strtok(NULL, " \t");
        }
        tokens[i] = NULL; // Null-terminate the tokens array

        pid_t pid = fork();
        if (pid == -1)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {
            // Child process
            execvp(tokens[0], tokens);
            perror("execvp");
            exit(EXIT_FAILURE);
        }
        else
        {
            // Parent process
            int status;
            waitpid(pid, &status, 0);

            printf("Executing: %s\n", input_copy);

            // Generate explanation based on the command
            if (strcmp(tokens[0], "cp") == 0)
            {
                if (i >= 3)
                {
                    printf("cp copies the file or directory from %s to %s.\n", tokens[1], tokens[2]);
                }
                else
                {
                    printf("cp copies files or directories.\n");
                }
            }
            else if (strcmp(tokens[0], "mkdir") == 0)
            {
                if (i >= 2)
                {
                    printf("mkdir creates a new directory named %s.\n", tokens[1]);
                }
                else
                {
                    printf("mkdir creates a new directory.\n");
                }
            }
            else if (strcmp(tokens[0], "grep") == 0)
            {
                if (i >= 3)
                {
                    printf("grep searches for the word %s in %s.\n", tokens[1], tokens[2]);
                }
                else
                {
                    printf("grep searches for a pattern in files.\n");
                }
            }
            else if (strcmp(tokens[0], "ls") == 0)
            {
                if (i == 1)
                {
                    printf("ls lists directory contents of the current directory.\n");
                }
                else
                {
                    printf("ls lists directory contents of ");
                    for (int j = 1; j < i; j++)
                    {
                        printf("%s", tokens[j]);
                        if (j < i - 1)
                        {
                            printf(", ");
                        }
                    }
                    printf(".\n");
                }
            }
        }
    }

    return 0;
}