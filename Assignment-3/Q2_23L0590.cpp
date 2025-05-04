#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <pthread.h>
#include <unistd.h>
#include <iostream>

using namespace std;

struct ThreadData
{
    char *start;
    char *end;
};

void *func(void *arg)
{
    ThreadData *data = (ThreadData *)arg;
    char *current = data->start;
    char *end = data->end;

    while (current <= end)
    {
        bool replaced = false;

        // Check for tan
        if (current + 2 <= end && strncmp(current, "tan", 3) == 0)
        {
            memcpy(current, "red", 3);
            current += 3;
            replaced = true;
        }
        // Check for olive
        else if (current + 4 <= end && strncmp(current, "olive", 5) == 0)
        {
            memcpy(current, "white", 5);
            current += 5;
            replaced = true;
        }
        // Check for black
        else if (current + 4 <= end && strncmp(current, "black", 5) == 0)
        {
            memcpy(current, "green", 5);
            current += 5;
            replaced = true;
        }

        if (!replaced)
        {
            current++;
        }
    }

    return nullptr;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cout << "Invalid arguments!" << endl;
        return 1;
    }

    int fd = open(argv[1], O_RDWR);
    if (fd == -1)
    {
        perror("open");
        return 1;
    }

    off_t fileSize = lseek(fd, 0, SEEK_END);

    char *fileData = (char *)mmap(nullptr, fileSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (fileData == MAP_FAILED)
    {
        perror("mmap");
        close(fd);
        return 1;
    }

    close(fd);

    pthread_t thread1, thread2;
    ThreadData data1, data2;

    int mid = fileSize / 2;
    data1.start = fileData;
    data1.end = fileData + mid - 1;
    data2.start = fileData + mid;
    data2.end = fileData + fileSize - 1;

    if (pthread_create(&thread1, nullptr, func, &data1) != 0)
    {
        perror("pthread_create1");
        return 1;
    }

    if (pthread_create(&thread2, nullptr, func, &data2) != 0)
    {
        perror("pthread_create2");
        return 1;
    }

    pthread_join(thread1, nullptr);
    pthread_join(thread2, nullptr);

    // cout.write(fileData, fileSize);
    int fd1 = open(argv[1], O_RDWR);
    if (fd == -1)
    {
        perror("open");
        return 1;
    }
    char buffer[fileSize];
    read(fd1, buffer, fileSize);
    cout << buffer << endl;

    munmap(fileData, fileSize);

    return 0;
}