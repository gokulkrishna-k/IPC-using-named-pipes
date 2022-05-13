#include <iostream>
#include <Windows.h>
#include <thread>

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

        response = serverWrite->WriteToPipe(message);
        if (!isRunning)
            return;
        if (response == ERROR_BROKEN_PIPE || response == ERROR_PIPE_NOT_CONNECTED)
        {
            ShowMessage("Error , Trying to Reconnect...", RED);

            serverWrite->disconnectPipe();
            serverRead->disconnectPipe();

            serverWrite->PipeConnect();
            serverRead->PipeConnect();

            continue;
        }
        else if (response == 1)
        {
            cout << "Shutting Down...\n";
            isRunning = false;
            return;
        }
        else
        {
            ShowMessage("Message Sent.\n", GREEN);
        }

        serverWrite->FlushBuffer();
    }
}

void ReadHandler(CustomNamedPipeServer *serverRead, CustomNamedPipeServer *serverWrite)
{
    int response;
    while (isRunning)
    {
        response = serverRead->ReadFromPipe();
        if (!isRunning)
            return;
        if (response == ERROR_BROKEN_PIPE || response == ERROR_PIPE_NOT_CONNECTED)
        {
            ShowMessage("Trying to Reconnect...\n", RED);
        }
        else if (response == 1)
        {
            cout << "Client has shutdown!  Waiting for new client...\n";
        }
        else if (response == 0)
        {
            ShowMessage("Message Received.\n", GREEN);
            continue;
        }
        else
        {
            cout << "Error in client!\nTrying to reconnect...\n";
        }
        serverRead->disconnectPipe();
        serverWrite->disconnectPipe();

        serverWrite->PipeConnect();
        serverRead->PipeConnect();
    }
}

int main()
{

    cout << "---- SERVER ----\n\n";

    CustomNamedPipeServer *serverRead = new CustomNamedPipeServer(
        TEXT("\\\\.\\pipe\\mypipe1"),
        PIPE_ACCESS_INBOUND,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES, 5000);

    CustomNamedPipeServer *serverWrite = new CustomNamedPipeServer(
        TEXT("\\\\.\\pipe\\mypipe2"),
        PIPE_ACCESS_OUTBOUND,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES, 5000);

    serverWrite->PipeConnect();
    serverRead->PipeConnect();

    thread writeThread(WriteHandler, serverRead, serverWrite);
    thread readThread(ReadHandler, serverRead, serverWrite);

    writeThread.join();
    readThread.join();

    serverWrite->disconnectPipe();
    serverRead->disconnectPipe();

    delete serverWrite;
    delete serverRead;

    cout << "Server Ended\n";
    return 1;
}