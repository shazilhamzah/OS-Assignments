#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>

int main()
{
    pid_t process1;
    pid_t process2;
    int decryptPID = 0;
    int redactPID = 0;
    char input;
    char input1[1000] = "";
    int i = 0;

    // printf("%s", "I am parent process ");
    // printf("%d\n", getpid());

    // GENERATION OF PROCESS 1
    process1 = fork();

    // THIS IF WILL BE HANDLED BY PROCESS 1
    if (!process1)
    {
        // STORING THE DATA INTO STRING "input1" OF LENGTH "1000".
        // CHARACTERS ADDED IN THE STRING IS "i"

        int fd = open("classified.txt", O_RDONLY);

        while ((read(fd, &input, 1)) > 0)
        {
            input1[i] = input;
            i++;
        }

        // DECRYPTING
        for (int j = 0; j < i; j++)
        {
            if ((input1[j] >= 97 && input1[j] < 123) || (input1[j] > 65 && input1[j] <= 90) || (input1[j] >= 48 && input1[j] <= 57))
            {
                if (input1[j] == 'a')
                {
                    input1[j] = '9';
                }
                else if (input1[j] == '0')
                {
                    input1[j] = '-';
                }
                else
                {
                    input1[j] = input1[j] - 1;
                }
            }
        }

        // CLOSING FILE
        close(fd);

        // WRITING INTO THE OUTPUT FILE
        int fd1 = open("decrypted.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644); // Open the output file

        for (int j = 0; j < i; j++)
        {
            write(fd1, &input1[j], 1);
        }
        close(fd1);

        // PRINTINGS
        // printf("%s\n", input1);
        // printf("%s", "I am process 1 ");
        // printf("%d\n", getpid());

        // EXITING THE PROCESS 1
        decryptPID = getpid();
        exit(getpid());
    }

    decryptPID = wait(NULL);

    // GENERATION OF PROCESS 2
    process2 = fork();

    // THIS IF WILL BE HANDLED BY PROCESS 2
    if (!process2)
    {
        // STORING THE DATA INTO STRING "input1" OF LENGTH "1000".
        // CHARACTERS ADDED IN THE STRING IS "i"
        i = 0;
        bool enterFlag = true;
        int check = 0;
        int fd = open("classified.txt", O_RDONLY);
        while ((read(fd, &input, 1)) > 0)
        {
            if (!enterFlag)
            {
                if (check >= 10)
                {
                    enterFlag = true;
                    check = 0;
                }
                check++;
            }
            if (i >= 5 && ((input >= 48 && input <= 57) || input == 'a') && input1[i - 1] == ' ' && input1[i - 2] == ':' && input1[i - 3] == 'O' && input1[i - 4] == 'T' && input1[i - 5] == 'T')
            {
                char conf[12] = "CONFIDENTIAL";

                // Insert "CONFIDENTIAL" into input1
                for (int k = 0; k < 12; k++)
                {
                    input1[i] = conf[k];
                    i++;
                }

                if (enterFlag)
                {
                    input1[i] = ',';
                    i++;
                    input1[i] = ' ';
                    i++;
                    enterFlag = false;
                }

                for (int digitCount = 0; digitCount < 11; digitCount++)
                {
                    read(fd, &input, 1);
                }
            }

            else
            {
                input1[i] = input;
                i++;
            }
        }

        // DECRYPTING
        for (int j = 0; j <= i; j++)
        {
            if (j + 12 <= i && input1[j] == 'C' && input1[j + 1] == 'O' && input1[j + 2] == 'N' && input1[j + 3] == 'F' && input1[j + 4] == 'I' && input1[j + 5] == 'D' && input1[j + 6] == 'E' && input1[j + 7] == 'N' && input1[j + 8] == 'T' && input1[j + 9] == 'I' && input1[j + 10] == 'A' && input1[j + 11] == 'L')
            {
                j += 12;
            }
            else if ((input1[j] >= 97 && input1[j] < 123) || (input1[j] > 65 && input1[j] <= 90) || (input1[j] >= 48 && input1[j] <= 57))
                if (input1[j] == 'a')
                {
                    input1[j] = '9';
                }
                else if (input1[j] == '0')
                {
                    input1[j] = '-';
                }
                else
                {
                    input1[j] = input1[j] - 1;
                }
        }

        // WRITING INTO THE OUTPUT FILE
        fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
        for (int j = 0; j < i; j++)
        {
            if (write(fd, &input1[j], 1) == -1)
            {
                perror("Error writing to file");
                close(fd);
                exit(getpid());
            }
        }
        close(fd);
        exit(getpid());
    }

    // OUTPUTS
    redactPID = wait(NULL);
    // printf("%d\n", decryptPID);
    // printf("%d\n", redactPID);
    // printf("%d\n", getpid());

    // APPENDING PID's TO OUTPUT.txt
    int fd;
    char s[255];
    int ret = sprintf(s, "\nDecrypted by child PID: %d, Redacted by child PID: %d, Parent PID: %d",
                      decryptPID, redactPID, getpid());
    fd = open("output.txt", O_WRONLY | O_APPEND | O_CREAT, 0644);
    int bytes_written = write(fd, s, ret);

    return 0;
}