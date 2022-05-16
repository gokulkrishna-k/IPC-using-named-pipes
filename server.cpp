#include <iostream>
#include <Windows.h>
#include <thread>
#define PIPE_TIMEOUT_CONNECT 10000
#define RETRY_LIMIT 5
using namespace std;

#include "util.h"
#include "custom-named-pipe-server.h"

bool isRunning = true;

void WriteHandler(CustomNamedPipeServer *serverRead, CustomNamedPipeServer *serverWrite)
{
    int response;
    while (isRunning)
    {
        char message[1023];
        cin.getline(message, 1023); // user inputs message...
        if (!isRunning)
            return;
        response = serverWrite->WriteToPipe(message);

        if (response == -1 || response == ERROR_BROKEN_PIPE || response == ERROR_PIPE_NOT_CONNECTED)
        {
            ShowMessage("Error , Trying to Reconnect...\n", RED);

            serverWrite->disconnectPipe();
            serverRead->disconnectPipe();

            BOOL connection;

            connection = serverWrite->PipeConnect();
            if (connection != FALSE)
            {
                connection = serverRead->PipeConnect();
            }
            if (connection == FALSE)
            {
                isRunning = false;
                return;
            }
            continue;
        }
        else if (response == 1)
        {
            cout << "Shutting Down...\n";
            isRunning = false;
            return;
        }
        else if (response == 0)
        {
            ShowMessage("Message Sent.\n", GREEN);
            serverWrite->FlushBuffer();
        }
    }
}

void ReadHandler(CustomNamedPipeServer *serverRead, CustomNamedPipeServer *serverWrite)
{
    int response;
    while (isRunning)
    {
        response = serverRead->ReadFromPipe();
        if (isRunning)
        {
            if (response == 0)
            {
                ShowMessage("Message Received.\n", GREEN);
                Sleep(500);
                continue;
            }
            else
            {
                if (response == ERROR_BROKEN_PIPE || response == ERROR_PIPE_NOT_CONNECTED)
                {
                    ShowMessage("Trying to Reconnect...\n", RED);
                }
                else if (response == 1)
                {
                    cout << "Client has shutdown!  Waiting for new client...\n";
                }
                else
                {
                    cout << "Error in client!\nTrying to reconnect...\n";
                }
                serverRead->disconnectPipe();
                serverWrite->disconnectPipe();

                BOOL connection;
                connection = serverWrite->PipeConnect();
                if (connection != FALSE)
                {
                    connection = serverRead->PipeConnect();
                }
                if (connection == FALSE)
                {
                    isRunning = false;
                    cin.ignore();
                    return;
                }
            }
        }
    }
}

int main()
{
    cout << "---- SERVER ----\n\n";

    CustomNamedPipeServer *serverRead = new CustomNamedPipeServer(
        TEXT("\\\\.\\pipe\\mypipe1"),
        PIPE_ACCESS_INBOUND | FILE_FLAG_OVERLAPPED,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES, 5000);

    CustomNamedPipeServer *serverWrite = new CustomNamedPipeServer(
        TEXT("\\\\.\\pipe\\mypipe2"),
        PIPE_ACCESS_OUTBOUND | FILE_FLAG_OVERLAPPED,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES, 5000);

    BOOL connection;

    connection = serverWrite->PipeConnect();
    if (connection != FALSE)
    {
        connection = serverRead->PipeConnect();
    }

    if (connection != FALSE)
    {
        thread writeThread(WriteHandler, serverRead, serverWrite);
        thread readThread(ReadHandler, serverRead, serverWrite);

        writeThread.join();
        readThread.join();

        // force quit
        serverWrite->disconnectPipe();
        serverRead->disconnectPipe();

        delete serverWrite;
        delete serverRead;

        cout << "Server Ended\n";
    }
    else
    {
        cout << "Server has shutdown due to timeout\n";
    }
    return 1;
}