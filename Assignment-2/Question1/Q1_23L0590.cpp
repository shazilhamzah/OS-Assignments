#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    pid_t pid;

    // Terminal command equivalent:
    // cat forensic_logs.txt | grep 'ALERT' | awk '!seen[$0]++' > forensic_report.txt

    int pipe1[2];
    int pipe2[2];
    pipe(pipe1); // THIS PIPE WILL CONNECT PERSON 1 AND 2 - PERSON 1 WILL WRITE
    pipe(pipe2); // THIS WILL CONNECT PERSON 2 AND 3 - PERSON 2 WILL WRITE

    // PERSON 1
    pid = fork();
    if (pid == 0)
    {
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
    if (pid == 0)
    {
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
    if (pid == 0)
    {
        close(pipe2[1]);
        dup2(pipe2[0], STDIN_FILENO);
        close(pipe2[0]);

        int out_fd = open("forensic_report.txt", O_WRONLY | O_CREAT, 0666);
        if (out_fd < 0)
        {
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

    for (int i = 0; i < 3; i++)
    {
        wait(NULL);
    }

    return 0;
}