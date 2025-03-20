#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
using namespace std;

#define FORENSIC_LOGS_SIZE 10000
#define FILTERED_DATA_SIZE 10000

int main()
{

    int fd1[2];
    int fd2[2];
    pipe(fd1); // THIS PIPE WILL CONNECT PERSON 1 AND 2 - PERSON 1 WILL WRITE
    pipe(fd2); // THIS WILL CONNECT PERSON 2 AND 3 - PERSON 2 WILL WRITE

    pid_t person1 = fork();
    if (person1 == 0)
    {
        // LINKING WITH FILE AND REDIRECTING INPUT
        int forensicFile = open("forensic_logs.txt", O_RDONLY);
        dup2(forensicFile, 0);

        // TAKING INPUT
        char buffer[FORENSIC_LOGS_SIZE];
        for (int i = 0; i < FORENSIC_LOGS_SIZE; i++)
        {
            cin >> noskipws >> buffer[i];
        }

        // PIPE SETTINGS
        close(fd1[0]);
        dup2(fd1[1], 1);
        cout << buffer;

        exit(0);
    }

    waitpid(person1, NULL, 0);
    pid_t person2 = fork();

    if (person2 == 0)
    {
        // LINKING WITH PIPE AND REDIRECTING INPUT
        int savedSTDIn = dup(STDIN_FILENO);
        close(fd1[1]);
        dup2(fd1[0], STDIN_FILENO);

        // TAKING INPUT FROM PIPE
        char buffer[FORENSIC_LOGS_SIZE];
        for (int i = 0; i < FORENSIC_LOGS_SIZE; i++)
        {
            cin >> noskipws >> buffer[i];
        }
        cout << endl
             << buffer << endl;

        dup2(savedSTDIn, STDIN_FILENO);
        close(savedSTDIn);

        // EXTRACTING OUT ALERT PART IN A BUFFER
        char filteredData[FILTERED_DATA_SIZE];
        int j = 0;
        int buffer_size = FORENSIC_LOGS_SIZE;

        for (int i = 0; i <= buffer_size - 5;)
        {
            if (buffer[i] == 'A' && buffer[i + 1] == 'L' && buffer[i + 2] == 'E' &&
                buffer[i + 3] == 'R' && buffer[i + 4] == 'T')
            {

                int start = i;

                while (i < buffer_size && buffer[i] != '\n')
                {
                    i++;
                }
                if (i < buffer_size && buffer[i] == '\n')
                {
                    i++;
                }

                for (int k = start; k < i && j < FILTERED_DATA_SIZE; k++, j++)
                {
                    filteredData[j] = buffer[k];
                }
            }
            else
                i++;
        }

        filteredData[j] = '\0';

        fflush(stdout);
        cout << endl
             << filteredData << endl;

        // WRITING ON PIPE 2
        close(fd2[0]);
        dup2(fd2[1], STDOUT_FILENO);
        // cout<<filteredData;

        // cout << "I am child 2 with PID " << getpid() << endl;
        exit(0);
    }

    // waitpid(person2,nullptr,0);
    pid_t person3 = fork();

    if (person3 == 0)
    {
        cout << "I am child 2 with PID " << getpid() << endl;
        exit(1);
    }

    return 0;
}