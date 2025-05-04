#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>

using namespace std;

#define SHM_NAME_BOOK "library_shm_book"
#define SHM_NAME_CLIENT "library_shm_client"
#define SEM_NAME_1 "library_sem1"
#define SEM_NAME_2 "library_sem2"
#define SEM_NAME_3 "library_sem3"

void getValidRollNumber(char name[100]);

int main()
{
    cout << "Welcome to library client!" << endl;

    key_t book_key = ftok(SHM_NAME_BOOK, 1024);
    int book_shmid = shmget(book_key, 1024, 0666);
    char *book_data = (char *)shmat(book_shmid, NULL, 0);
    if (book_data && book_data[0] != '\0')
    {
        cout << "Available Books:\n"
             << book_data << endl;
    }
    else
    {
        cout << "No books available in the library!\n";
    }
    shmdt(book_data);

    key_t req_key = ftok(SHM_NAME_CLIENT, 1);
    int req_shmid = shmget(req_key, 1024, 0666);
    char *request_data = (char *)shmat(req_shmid, NULL, 0);

    key_t resp_key = ftok(SHM_NAME_CLIENT, 2);
    int resp_shmid = shmget(resp_key, 1024, 0666);
    char *response_data = (char *)shmat(resp_shmid, NULL, 0);

    sem_t *sem_request = sem_open(SEM_NAME_1, 0);
    sem_t *sem_response = sem_open(SEM_NAME_2, 0);
    sem_t *sem_mutex = sem_open(SEM_NAME_3, 0);

    char name[100], func[7], bookName[100];
    int quantity;

    getValidRollNumber(name);
    // cout << "Enter your Roll Number (XXLXXXX): ";
    // cin.getline(name, 100);

    while (true)
    {
        cout << "Hi " << name << "! Would you like to borrow or return? ";
        cin >> func;
        cin.ignore();
        if (strcmp(func, "borrow") == 0 || strcmp(func, "return") == 0)
            break;
        cout << "Invalid function " << func << endl;
    }

    cout << "\nEnter book name: ";
    cin.getline(bookName, 100);
    cout << "Enter quantity: ";
    cin >> quantity;

    char info[1000];
    snprintf(info, sizeof(info), "%s,%c,%d,%s", name, (func[0] == 'b') ? 'b' : 'r', quantity, bookName);

    sem_wait(sem_mutex);
    strcpy(request_data, info);
    sem_post(sem_mutex);

    sem_post(sem_request);
    sem_wait(sem_response);

    char message[1024];
    strncpy(message, response_data, sizeof(message) - 1);
    cout << "\n\nServer Response: " << endl
         << response_data << endl;
    memset(response_data, 0, 1024);

    shmdt(request_data);
    shmdt(response_data);
    sem_close(sem_request);
    sem_close(sem_response);
    sem_close(sem_mutex);

    return 0;
}

void getValidRollNumber(char name[100])
{
    bool isValid = false;

    while (!isValid)
    {
        cout << "Enter your Roll Number (XXLXXXX where X is a digit): ";
        cin.getline(name, 100);

        // Check length first (must be exactly 7 characters)
        if (strlen(name) != 7)
        {
            cout << "Error: Roll Number must be exactly 7 characters long.\n";
            continue;
        }

        // Check each character
        isValid = true;
        for (int i = 0; i < 7; i++)
        {
            // First two characters must be digits
            if (i < 2 && !isdigit(name[i]))
            {
                isValid = false;
                break;
            }
            // Third character must be 'L'
            else if (i == 2 && name[i] != 'L')
            {
                isValid = false;
                break;
            }
            // Last four characters must be digits
            else if (i > 2 && !isdigit(name[i]))
            {
                isValid = false;
                break;
            }
        }

        if (!isValid)
        {
            cout << "Error: Invalid format. Please use XXLXXXX format where X is a digit (0-9).\n";
        }
    }
}