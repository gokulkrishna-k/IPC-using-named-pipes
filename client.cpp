#include <windows.h>
#include <iostream>
#include <thread>

using namespace std;

#include "util.h"
#include "custom-named-pipe-client.h"

char closeMessage[100] = "close";
bool isRunning = true;

void WriteHandler(CustomNamedPipeClient *clientRead, LPTSTR readPipeName, CustomNamedPipeClient *clientWrite, LPTSTR writePipeName)
{

    int response;

    while (isRunning)
    {

        char message[1023];
        cin.getline(message, 1023);

        response = clientWrite->WriteToPipe(message);
        if (!isRunning)
            return;
        if (response == ERROR_BROKEN_PIPE || response == ERROR_PIPE_NOT_CONNECTED)
        {
            ShowMessage("Error , Trying to Reconnect...", RED);
            clientWrite->OpenServerConnection(writePipeName, GENERIC_WRITE);
            clientRead->OpenServerConnection(readPipeName, GENERIC_READ);
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
        cout << "Read..\n";
        response = clientRead->ReadFromPipe();
        if (!isRunning)
            return;
        if (response == ERROR_BROKEN_PIPE || response == ERROR_PIPE_NOT_CONNECTED)
        {
            cout << "Error in accessing server!\nTrying to reconnect...\n";
            clientRead->OpenServerConnection(readPipeName, GENERIC_READ);
            clientWrite->OpenServerConnection(writePipeName, GENERIC_WRITE);
        }
        else if (response == 1)
        {
            ShowMessage("Server initiated Shutdown.\n", GREEN);
            isRunning = false;
            break;
        }
        else
        {
            ShowMessage("Message Received.\n", GREEN);
        }
    }
}

int main()
{
    cout << "---- CLIENT ----\n\n";

    CustomNamedPipeClient *clientRead = new CustomNamedPipeClient();
    CustomNamedPipeClient *clientWrite = new CustomNamedPipeClient();

    LPTSTR writePipeName = TEXT("\\\\.\\pipe\\mypipe1");
    LPTSTR readPipeName = TEXT("\\\\.\\pipe\\mypipe2");

    clientRead->OpenServerConnection(readPipeName, GENERIC_READ);
    Sleep(500);
    clientWrite->OpenServerConnection(writePipeName, GENERIC_WRITE);

    thread readThread(ReadHandler, clientRead, readPipeName, clientWrite, writePipeName);
    thread writeThread(WriteHandler, clientRead, readPipeName, clientWrite, writePipeName);

    readThread.join();
    writeThread.join();

    delete clientWrite;
    delete clientRead;

    cout << "\nClient Ended\n";
    return 0;
}