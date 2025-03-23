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

void extractBooks(char buffer[1000], int &start, char temp[1000], vector<Books> &books);
string findBookByName(vector<Books> &books, const string bookName, int quantity, char function);
void writeToFile(vector<Books> &books);

int main()
{
    cout << "Hello, from Server!" << endl;
    int booksFileRead = open("books.txt", O_RDONLY);
    char buffer[1000];
    read(booksFileRead, buffer, 1000);

    vector<Books> books;
    char temp[1000];
    int start = 0;
    extractBooks(buffer, start, temp, books);
    mkfifo(FIFO_PATH, 0666);

    while (1)
    {
        cout << "Waiting for client request..." << endl;
        char name[100];
        char func;
        char bookName[100];
        int quantity;

        int fifoRead = open(FIFO_PATH, O_RDONLY);
        char info[1000];

        read(fifoRead, info, 1000);
        cout << "Client connected: " << info << endl;
        strcpy(name, info);

        read(fifoRead, info, 1000);

        int startPos = strlen(name) + 1;

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

        // cout << "Function: " << func << "\n";
        // cout << "Quantity: " << quantity << "\n";
        // cout << "Book Name: " << bookName << endl;

        close(fifoRead);
        string message = findBookByName(books, bookName, quantity, func);
        // cout << message;

        char messageBuffer[100];
        strcpy(messageBuffer, message.c_str());
        int fifoWrite = open(FIFO_PATH, O_WRONLY);
        write(fifoWrite, messageBuffer, 100);
        close(fifoWrite);

        for (auto book : books)
        {
            for (auto name : book.name)
            {
                cout << name << " ";
            }
            cout << book.quantity << endl;
        }

        writeToFile(books);
    }

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
            cout << "Book found!" << endl;
            if (function == 'b') // Borrow operation
            {
                if (book.quantity >= quantity)
                {
                    message = to_string(quantity) + " copies of " + bookName + " borrowed successfully!\n";
                    book.quantity -= quantity; // Update quantity here
                }
                else
                {
                    message = "Not enough quantity! You will get quantity: " + to_string(book.quantity) + "\n";
                    book.quantity = 0; // Set to zero if not enough books are available
                }
            }
            else // Return operation
            {
                cout << "Am in here!" << endl;
                message = to_string(quantity) + " copies of " + bookName + " returned successfully!\n";
                book.quantity += quantity; // Update quantity here
            }
            return message;
        }
    }

    // If book not found, return appropriate error message
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
    for (auto i : books)
    {
        for (const auto &word : i.name)
        {
            result += word + " ";
        }
        result += to_string(i.quantity) + "\n";
    }
    strncpy(buffer, result.c_str(), 1000);
    buffer[999] = '\0';
    int booksFileWrite = open("books.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    write(booksFileWrite, buffer, strlen(buffer));
    cout << sizeof(buffer);
    return;
}