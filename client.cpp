#include <windows.h>
#include <iostream>
#include <thread>
#include <stdlib.h>
using namespace std;
#define RETRY_LIMIT 5
#define BUFF_SIZE 1023
#include "util.h"

#include "custom-named-pipe-client.h"

bool isRunning = true;

void WriteHandler(CustomNamedPipeClient *clientRead, LPTSTR readPipeName, CustomNamedPipeClient *clientWrite, LPTSTR writePipeName)
{

    int response;

    while (isRunning)
    {

        char message[BUFF_SIZE];

        cin.getline(message, BUFF_SIZE);
        cin.clear();
        cin.ignore(INT_MAX, '\n');

        if (!isRunning)
            return;
        response = clientWrite->WriteToPipe(message);
        if (response == ERROR_BROKEN_PIPE || response == ERROR_PIPE_NOT_CONNECTED)
        {
            ShowMessage("Error , Trying to Reconnect...", RED);
            BOOL connection = clientRead->OpenServerConnection(readPipeName, GENERIC_READ);
            Sleep(100);
            if (connection != FALSE)
                clientWrite->OpenServerConnection(writePipeName, GENERIC_WRITE);

            if (connection == FALSE)
            {
                ShowMessage("Reconnection Failed. Closing Client\n", GREEN);
                isRunning = false;
                // using exit to force quit the blocking cin
                delete clientWrite;
                delete clientRead;
                exit(0);
            }
            continue;
        }
        else if (response == 1)
        {
            cout << "Shutting Down...\n";
            isRunning = false;
            break;
        }
        else
        {
            ShowMessage("Message Sent.\n", GREEN);
        }
    }
}

void ReadHandler(CustomNamedPipeClient *clientRead, LPTSTR readPipeName, CustomNamedPipeClient *clientWrite, LPTSTR writePipeName)
{
    int response;

    while (isRunning)
    {
        response = clientRead->ReadFromPipe();
        if (!isRunning)
            return;
        if (response == ERROR_BROKEN_PIPE || response == ERROR_PIPE_NOT_CONNECTED)
        {
            cout << "Error in accessing server!\nTrying to reconnect...\n";
            BOOL connection;

            connection = clientRead->OpenServerConnection(readPipeName, GENERIC_READ);
            Sleep(100);
            if (connection != FALSE)
                clientWrite->OpenServerConnection(writePipeName, GENERIC_WRITE);

            if (connection == FALSE)
            {
                ShowMessage("Reconnection Failed. Closing Client\n", GREEN);
                isRunning = false;
                // using exit to force quit the blocking cin
                delete clientWrite;
                delete clientRead;
                exit(0);
            }
        }
        else if (response == 1)
        {
            ShowMessage("Server initiated Shutdown.\n", GREEN);
            isRunning = false;
            // using exit to force quit the blocking cin
            delete clientWrite;
            delete clientRead;
            exit(0);
        }
        else
        {
            ShowMessage("Message Received.\n", GREEN);
        }
    }
}

int main()
{

    system("CLS");
    cout << "---- CLIENT ----\n\n";

    CustomNamedPipeClient *clientRead = new CustomNamedPipeClient();
    CustomNamedPipeClient *clientWrite = new CustomNamedPipeClient();

    LPTSTR writePipeName = TEXT("\\\\.\\pipe\\mypipe1");
    LPTSTR readPipeName = TEXT("\\\\.\\pipe\\mypipe2");

    BOOL connection;

    connection = clientRead->OpenServerConnection(readPipeName, GENERIC_READ);
    Sleep(100);
    if (connection != FALSE)
        clientWrite->OpenServerConnection(writePipeName, GENERIC_WRITE);

    if (connection != FALSE)
    {
        thread readThread(ReadHandler, clientRead, readPipeName, clientWrite, writePipeName);
        thread writeThread(WriteHandler, clientRead, readPipeName, clientWrite, writePipeName);

        readThread.join();
        writeThread.join();

        delete clientWrite;
        delete clientRead;

        cout << "\nClient Ended\n";
    }
    else
    {
        cout << "Client ended due to Timeout!\n";
    }

    return 0;
}