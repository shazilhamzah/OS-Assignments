// #include <unistd.h>
// #include <sys/wait.h>
// #include <fcntl.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <sys/stat.h>
// #include <iostream>
// #include <vector>
// #include <string.h>
// #include <sstream>

// using namespace std;

// #define FIFO_PATH "library_fifo"
// #define MAX_BOOKS 10

// struct Books
// {
//     vector<string> name;
//     int quantity;
// };

// int main()
// {
//     cout << "Hello, from Server!" << endl;
//     cout << "Waiting for client request..." << endl;
//     char name[100];
//     char func;
//     char bookName[100];
//     int quantity;

//     // READING DATA FROM books.txt
//     int booksFile = open("books.txt", O_RDONLY);
//     char buffer[1000];
//     read(booksFile, buffer, 1000);
//     // cout << buffer << endl;

//     // TOKENIZATION INTO books VECTOR
//     vector<Books> books;
//     char temp[1000];
//     int start = 0;
//     for (int i = 0; i < 1000; i++)
//     {
//         if (buffer[i] == '\n' || buffer[i] == '\0')
//         {
//             int n = 0;
//             for (int j = start; j < i; j++)
//             {
//                 temp[n] = buffer[j];
//                 n++;
//             }
//             temp[n] = '\0';
//             start = i + 1;

//             Books book;
//             stringstream ss(temp);
//             string word;
//             while (ss >> word)
//             {
//                 book.name.push_back(word);
//             }

//             book.quantity = atoi(book.name.back().c_str()); // c_str converts string to char* required by atoi
//             book.name.pop_back();                           // removing the quantity part

//             books.push_back(book);

//             if (buffer[i] == '\0')
//                 break;
//         }
//     }

//     // PRINTING BOOKS
//     for (auto &i : books)
//     {
//         for (auto &j : i.name)
//         {
//             cout << j << " ";
//         }
//         cout << i.quantity << endl;
//     }

//     // MAKING AND GETTING INFO FROM FIFO
//     mkfifo(FIFO_PATH, 0666);
//     int fifoRead = open(FIFO_PATH, O_RDONLY);
//     char info[1000];
//     read(fifoRead, info, 1000);
//     // cout << info << endl;

//     int startPos = 0;

//     for (int i = 0; info[i] != ',' && info[i] != '\0'; i++)
//     {
//         name[i] = info[i];
//         startPos = i + 1;
//     }
//     startPos++;
//     name[startPos - 1] = '\0';

//     func = info[startPos];
//     startPos += 2;

//     quantity = 0;
//     for (int i = startPos; info[i] != ',' && info[i] != '\0'; i++)
//     {
//         quantity = quantity * 10 + (info[i] - '0');
//         startPos++;
//     }
//     startPos++;

//     int bookNameIndex = 0;
//     for (int i = startPos; info[i] != '\0'; i++)
//     {
//         bookName[bookNameIndex++] = info[i];
//     }
//     bookName[bookNameIndex] = '\0';

//     // PRINTING EXTRACTED DATA
//     // cout << "Name: " << name << "\n";
//     // cout << "Function: " << func << "\n";
//     // cout << "Quantity: " << quantity << "\n";
//     // cout << "Book Name: " << bookName << endl;

//     cout << "Client connected: " << name << endl;
//     return 0;
// }

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

using namespace std;

#define FIFO_PATH "library_fifo"
#define MAX_BOOKS 10

struct Books
{
    vector<string> name;
    int quantity;
};

int main()
{
    cout << "Hello, from Server!" << endl;
    cout << "Waiting for client request..." << endl;
    char name[100];
    char func;
    char bookName[100];
    int quantity;

    int booksFile = open("books.txt", O_RDONLY);
    char buffer[1000];
    read(booksFile, buffer, 1000);

    vector<Books> books;
    char temp[1000];
    int start = 0;
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

    mkfifo(FIFO_PATH, 0666);
    int fifoRead = open(FIFO_PATH, O_RDONLY);
    char info[1000];

    read(fifoRead, info, 1000);
    cout << "Client connected: " << info << endl;
    strcpy(name, info);

    read(fifoRead, info, 1000);

    int startPos = strlen(name)+1;

    func = info[startPos];
    startPos += 2;

    quantity = 0;
    for (int i = startPos; info[i] != ',' && info[i] != '\0'; i++)
    {
        quantity = quantity * 10 + (info[i] - '0');
        startPos++;
    }
    startPos++;

    int bookNameIndex = 0;
    for (int i = startPos; info[i] != '\0'; i++)
    {
        bookName[bookNameIndex++] = info[i];
    }
    bookName[bookNameIndex] = '\0';

    cout << "Function: " << func << "\n";
    cout << "Quantity: " << quantity << "\n";
    cout << "Book Name: " << bookName << endl;

    return 0;
}
