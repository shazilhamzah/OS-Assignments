#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <iostream>
#include <vector>
#include <string.h>
#include <sstream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <semaphore.h>

using namespace std;

#define MAX_BOOKS 10
#define SHM_NAME_BOOK "library_shm_book"
#define SHM_NAME_CLIENT "library_shm_client"
#define SEM_NAME_1 "library_sem1"
#define SEM_NAME_2 "library_sem2"
#define SEM_NAME_3 "library_sem3"

struct Books
{
    vector<string> name;
    int quantity;
};

void extractBooks(char buffer[1000], int &start, char temp[1000], vector<Books> &books);
string findBookByName(vector<Books> &books, const string bookName, int quantity, char function);
void writeToFile(vector<Books> &books);

int main()
{
    cout << "Hello, from Server!" << endl;
    int booksFileRead = open("books.txt", O_RDONLY);
    char buffer[1000];
    read(booksFileRead, buffer, 1000);
    close(booksFileRead);

    vector<Books> books;
    char temp[1000];
    int start = 0;
    extractBooks(buffer, start, temp, books);

    key_t book_key = ftok(SHM_NAME_BOOK, 1024);
    int book_shmid = shmget(book_key, 1024, IPC_CREAT | 0666);
    char *book_data = (char *)shmat(book_shmid, NULL, 0);
    strcpy(book_data, buffer);
    shmdt(book_data);

    key_t req_key = ftok(SHM_NAME_CLIENT, 1);
    int req_shmid = shmget(req_key, 1024, IPC_CREAT | 0666);
    char *request_data = (char *)shmat(req_shmid, NULL, 0);

    key_t resp_key = ftok(SHM_NAME_CLIENT, 2);
    int resp_shmid = shmget(resp_key, 1024, IPC_CREAT | 0666);
    char *response_data = (char *)shmat(resp_shmid, NULL, 0);

    sem_t *sem_request = sem_open(SEM_NAME_1, O_CREAT, 0666, 0);
    sem_t *sem_response = sem_open(SEM_NAME_2, O_CREAT, 0666, 0);
    sem_t *sem_mutex = sem_open(SEM_NAME_3, O_CREAT, 0666, 1);

    while (1)
    {
        int booksFileRead = open("books.txt", O_RDONLY);
        char buffer[1000] = {0};
        ssize_t bytes_read = read(booksFileRead, buffer, 999); // Leave space for null
        if (bytes_read > 0)
            buffer[bytes_read] = '\0';
        close(booksFileRead);

        books.clear();
        char temp[1000];
        int start = 0;
        extractBooks(buffer, start, temp, books);

        // Write fresh data to shared memory
        key_t book_key = ftok(SHM_NAME_BOOK, 1024);
        int book_shmid = shmget(book_key, 1024, IPC_CREAT | 0666);
        char *book_data = (char *)shmat(book_shmid, NULL, 0);
        strcpy(book_data, buffer);
        shmdt(book_data);

        cout << "Waiting for client request..." << endl;
        sem_wait(sem_request);

        char info[1000];
        strcpy(info, request_data);

        char name[100], bookName[100], func;
        int quantity;
        char *token = strtok(info, ",");
        strcpy(name, token);
        token = strtok(NULL, ",");
        func = token[0];
        token = strtok(NULL, ",");
        quantity = atoi(token);
        token = strtok(NULL, ",");
        strcpy(bookName, token);

        cout << "Client connected: " << name << endl;
        string message = findBookByName(books, bookName, quantity, func);

        char messageBuffer[1024] = {0};
        strncpy(messageBuffer, message.c_str(), sizeof(messageBuffer) - 1);
        strcpy(response_data, messageBuffer);
        sem_post(sem_response);

        memset(request_data, 0, 1024);
        // memset(response_data, 0, 1024);

        writeToFile(books);
    }

    shmdt(request_data);
    shmdt(response_data);
    shmctl(req_shmid, IPC_RMID, NULL);
    shmctl(resp_shmid, IPC_RMID, NULL);
    sem_close(sem_request);
    sem_close(sem_response);
    sem_close(sem_mutex);
    sem_unlink(SEM_NAME_1);
    sem_unlink(SEM_NAME_2);
    sem_unlink(SEM_NAME_3);

    return 0;
}

void extractBooks(char buffer[1000], int &start, char temp[1000], std::vector<Books> &books)
{
    for (int i = 0; i < 1000; i++)
    {
        if (buffer[i] == '\n' || buffer[i] == '\0')
        {
            int n = 0;
            for (int j = start; j < i; j++)
            {
                temp[n] = buffer[j];
                n++;
            }
            temp[n] = '\0';
            start = i + 1;

            Books book;
            stringstream ss(temp);
            string word;
            while (ss >> word)
            {
                book.name.push_back(word);
            }

            book.quantity = atoi(book.name.back().c_str());
            book.name.pop_back();

            books.push_back(book);

            if (buffer[i] == '\0')
                break;
        }
    }
}

string findBookByName(vector<Books> &books, const string bookName, int quantity, char function)
{
    string message;
    for (auto &book : books) // Using reference to modify the original book object
    {
        string fullBookName = "";
        for (auto word : book.name)
        {
            fullBookName += word + " ";
        }
        // REMOVING EXTRA SPACE
        if (!fullBookName.empty())
        {
            fullBookName.pop_back();
        }

        if (fullBookName == bookName)
        {
            if (function == 'b')
            {
                if (book.quantity >= quantity)
                {
                    message = to_string(quantity) + " copies of " + bookName + " borrowed successfully!\n";
                    book.quantity -= quantity;
                }
                else
                {
                    message = "Not enough quantity! You will get quantity: " + to_string(book.quantity) + "\n";
                    book.quantity = 0;
                }
            }
            else
            {
                message = to_string(quantity) + " copies of " + bookName + " returned successfully!\n";
                book.quantity += quantity;
            }
            return message;
        }
    }

    if (function == 'b')
    {
        message = "Error: " + bookName + " does not exist in the library. Cannot borrow a non-existing book.\n";
    }
    else
    {
        message = "Error: " + bookName + " does not exist in the library. Cannot return a non-existing book.\n";
    }

    return message;
}

void writeToFile(vector<Books> &books)
{
    char buffer[1000];
    string result;
    for (size_t index = 0; index < books.size(); ++index)
    {
        for (const auto &word : books[index].name)
        {
            result += word + " ";
        }
        result += to_string(books[index].quantity);

        if (index != books.size() - 1) // Avoid adding '\n' after the last entry
        {
            result += "\n";
        }
    }

    strncpy(buffer, result.c_str(), 1000);
    buffer[999] = '\0';

    int booksFileWrite = open("books.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    write(booksFileWrite, buffer, strlen(buffer));
    close(booksFileWrite);

    key_t book_key = ftok(SHM_NAME_BOOK, 1024);
    int book_shmid = shmget(book_key, 1024, IPC_CREAT | 0666);
    char *book_data = (char *)shmat(book_shmid, NULL, 0);
    strncpy(book_data, buffer, 1000);
    book_data[999] = '\0';
    shmdt(book_data);
}
