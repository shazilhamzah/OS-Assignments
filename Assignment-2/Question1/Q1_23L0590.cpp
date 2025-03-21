#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    pid_t pid;

    // Terminal command equivalent:
    // cat forensic_logs.txt | grep 'ALERT' | awk '!seen[$0]++' > forensic_report.txt

    int pipe1[2];
    int pipe2[2];
    pipe(pipe1); // THIS PIPE WILL CONNECT PERSON 1 AND 2 - PERSON 1 WILL WRITE
    pipe(pipe2); // THIS WILL CONNECT PERSON 2 AND 3 - PERSON 2 WILL WRITE


    // PERSON 1
    pid = fork();
    if (pid == 0) {
        close(pipe1[0]);
        dup2(pipe1[1], STDOUT_FILENO);
        close(pipe1[1]);

        execlp("cat", "cat", "forensic_logs.txt", NULL);
        perror("execlp cat failed");
        exit(EXIT_FAILURE);
    }
    close(pipe1[1]);


    // PERSON 2
    pid = fork();
    if (pid == 0) {
        close(pipe2[0]);
        dup2(pipe1[0], STDIN_FILENO);
        close(pipe1[0]);
        dup2(pipe2[1], STDOUT_FILENO);
        close(pipe2[1]);

        execlp("grep", "grep", "ALERT", NULL);
        perror("execlp grep failed");
        exit(EXIT_FAILURE);
    }
    close(pipe1[0]);
    close(pipe2[1]); 

    // PERSON 3
    pid = fork();
    if (pid == 0) {
        close(pipe2[1]);
        dup2(pipe2[0], STDIN_FILENO);
        close(pipe2[0]);

        int out_fd = open("forensic_report.txt", O_WRONLY | O_CREAT , 0666);
        if (out_fd < 0) {
            perror("open forensic_report.txt failed");
            exit(EXIT_FAILURE);
        }
        dup2(out_fd, STDOUT_FILENO);
        close(out_fd);

        execlp("awk", "awk", "!seen[$0]++", NULL);
        perror("execlp awk failed");
        exit(EXIT_FAILURE);
    }
    close(pipe2[0]);

    for (int i = 0; i < 3; i++) {
        wait(NULL);
    }

    return 0;
}

// #include <iostream>
// #include <stdlib.h>
// #include <unistd.h>
// #include <fcntl.h>
// #include <sys/wait.h>
// using namespace std;

// #define FORENSIC_LOGS_SIZE 10000
// #define FILTERED_DATA_SIZE 10000

// int main()
// {

//     int fd1[2];
//     int fd2[2];
//     pipe(fd1); // THIS PIPE WILL CONNECT PERSON 1 AND 2 - PERSON 1 WILL WRITE
//     pipe(fd2); // THIS WILL CONNECT PERSON 2 AND 3 - PERSON 2 WILL WRITE

//     pid_t person1 = fork();
//     if (person1 == 0)
//     {
//         // LINKING WITH FILE AND REDIRECTING INPUT
//         int forensicFile = open("forensic_logs.txt", O_RDONLY);
//         dup2(forensicFile, 0);

//         // TAKING INPUT
//         char buffer[FORENSIC_LOGS_SIZE];
//         for (int i = 0; i < FORENSIC_LOGS_SIZE; i++)
//         {
//             cin >> noskipws >> buffer[i];
//         }

//         // PIPE SETTINGS
//         close(fd1[0]);
//         dup2(fd1[1], 1);
//         cout << buffer;

//         exit(0);
//     }

//     waitpid(person1, NULL, 0);
//     pid_t person2 = fork();

//     if (person2 == 0)
//     {
//         // LINKING WITH PIPE AND REDIRECTING INPUT
//         int savedSTDIn = dup(STDIN_FILENO);
//         close(fd1[1]);
//         dup2(fd1[0], STDIN_FILENO);

//         // TAKING INPUT FROM PIPE
//         char buffer[FORENSIC_LOGS_SIZE];
//         for (int i = 0; i < FORENSIC_LOGS_SIZE; i++)
//         {
//             cin >> noskipws >> buffer[i];
//         }
//         cout << endl
//              << buffer << endl;

//         dup2(savedSTDIn, STDIN_FILENO);
//         close(savedSTDIn);

//         // EXTRACTING OUT ALERT PART IN A BUFFER
//         char filteredData[FILTERED_DATA_SIZE];
//         int j = 0;
//         int buffer_size = FORENSIC_LOGS_SIZE;

//         for (int i = 0; i <= buffer_size - 5;)
//         {
//             if (buffer[i] == 'A' && buffer[i + 1] == 'L' && buffer[i + 2] == 'E' &&
//                 buffer[i + 3] == 'R' && buffer[i + 4] == 'T')
//             {

//                 int start = i;

//                 while (i < buffer_size && buffer[i] != '\n')
//                 {
//                     i++;
//                 }
//                 if (i < buffer_size && buffer[i] == '\n')
//                 {
//                     i++;
//                 }

//                 for (int k = start; k < i && j < FILTERED_DATA_SIZE; k++, j++)
//                 {
//                     filteredData[j] = buffer[k];
//                 }
//             }
//             else
//                 i++;
//         }

//         filteredData[j] = '\0';

//         fflush(stdout);
//         cout << endl
//              << filteredData << endl;

//         // WRITING ON PIPE 2
//         close(fd2[0]);
//         dup2(fd2[1], STDOUT_FILENO);
//         cout << filteredData;

//         cout << "I am child 2 with PID " << getpid() << endl;
//         exit(0);
//     }

//     pid_t person3 = fork();

//     if (person3 == 0)
//     {
//         // LINKING WITH PIPE AND REDIRECTING INPUT
//         int savedSTDIn = dup(STDIN_FILENO);
//         close(fd2[1]);
//         dup2(fd2[0], STDIN_FILENO);

//         // TAKING INPUT FROM PIPE
//         char buffer[FORENSIC_LOGS_SIZE];
//         for (int i = 0; i < FORENSIC_LOGS_SIZE; i++)
//         {
//             cin >> noskipws >> buffer[i];
//         }
//         cout << endl
//              << buffer << endl;

//         dup2(savedSTDIn, STDIN_FILENO);
//         close(savedSTDIn);

//         cout << "I am child 3 with PID " << getpid() << endl;
//         exit(0);
//     }

//     // waitpid(person2, NULL, 0);
//     waitpid(person3, NULL, 0);

//     cout << "I am parent with PID " << getpid() << endl;

//     return 0;
// }
