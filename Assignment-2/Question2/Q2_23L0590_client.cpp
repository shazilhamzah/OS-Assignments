#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <iostream>
#include <vector>
#include <cstring>
using namespace std;

#define FIFO_PATH "library_fifo"

int main()
{
    cout << "Welcome to library client!" << endl;

    int fifoWrite = open(FIFO_PATH, O_WRONLY);

    char name[100];
    char func[7];
    char bookName[100];
    int quantity;

    cout << "Enter your name: ";
    cin.getline(name, 100);

    write(fifoWrite, name, strlen(name) + 1);

    cout << "Waiting for server acknowledgment..." << endl;

    while (true)
    {
        cout << "Hi " << name << "! Would you like to borrow or return? ";
        cin >> func;
        cin.ignore();
        if (strcmp(func, "borrow") == 0 || strcmp(func, "return") == 0)
        {
            break;
        }
        else
        {
            cout << "Invalid function " << func << endl;
        }
    }

    cout << "\nEnter book name: ";
    cin.getline(bookName, 100);
    cout << "Enter quantity: ";
    cin >> quantity;

    char info[1000];
    int n = 0;

    for (int i = 0; i < 100; i++)
    {
        if (name[i] == '\0')
        {
            break;
        }
        info[n] = name[i];
        n++;
    }
    info[n++] = ',';

    if (strcmp(func, "borrow") == 0)
    {
        info[n++] = 'b';
    }
    else
    {
        info[n++] = 'r';
    }
    info[n++] = ',';

    string qty_str = to_string(quantity);
    for (char c : qty_str)
    {
        info[n++] = c;
    }
    info[n++] = ',';

    for (int i = 0; bookName[i] != '\0'; i++)
        info[n++] = bookName[i];

    info[n] = '\0';

    write(fifoWrite, info, strlen(info) + 1);

    close(fifoWrite);
    int fifoRead = open(FIFO_PATH, O_RDONLY);
    char message[100];
    read(fifoRead, message, 100);
    cout << message << endl;

    return 0;
}
