#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <iostream>
#include <vector>
using namespace std;

typedef struct
{
    int start;
    int end;
} thread_data;

int isPrime(int n)
{
    if (n <= 1)
        return 0;
    if (n == 2)
        return 1;
    if (n % 2 == 0)
        return 0;
    for (int i = 3; i * i <= n; i += 2)
    {
        if (n % i == 0)
            return 0;
    }
    return 1;
}

// THREAD FUNCTION
void *func(void *arg)
{
    thread_data *data = (thread_data *)arg;

    vector<int> v;
    for (int i = data->start; i <= data->end; i++)
    {
        if (isPrime(i))
        {
            v.push_back(i);
        }
    }

    cout << "Thread " << gettid() << " ";
    cout << "(" << data->start << "-" << data->end << "): ";
    for (auto i : v)
        cout << i << " ";
    cout << endl;
    fflush_unlocked(stdout);
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    if (argc < 3 || argc > 3)
    {
        cout << "Invalid arguments!" << endl;
        return 0;
    }
    int L = atoi(argv[1]);
    int R = atoi(argv[2]);

    if (L > R)
    {
        cout << "Second argument must be greated than the first one!" << endl;
        return 0;
    }

    int noOfThreads = R - L;
    noOfThreads = noOfThreads / 10;
    noOfThreads = noOfThreads + 1;

    pthread_t tid[noOfThreads];
    thread_data data[noOfThreads];

    for (int i = 0; i < noOfThreads; i++)
    {
        data[i].start = L + i * 10;
        data[i].end = data[i].start + 9;
        if (data[i].end > R)
        {
            data[i].end = R;
        }
        if (data[i].start > R)
        {
            data[i].start = R;
            data[i].end = R;
        }

        pthread_create(&tid[i], NULL, func, &data[i]);
    }

    for (int i = 0; i < noOfThreads; i++)
    {
        pthread_join(tid[i], NULL);
    }

    return 0;
}