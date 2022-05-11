#include <iostream>
#include <Windows.h>
#include <thread>

using namespace std;

#include "util.h"
#include "custom-named-pipe-server.h"

int main()
{

    cout << "---- SERVER ----\n\n";
    CustomNamedPipeServer server(
        TEXT("\\\\.\\pipe\\mypipe"),
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES, 0);

    server.PipeConnect();
    int response;
    while (1)
    {
        char message[1023];
        cout << "Enter Message : ";
        cin.getline(message, 1023);

        response = server.WriteToPipe(message);

        if (response == -1)
        {
            ShowMessage("Error , Trying to Reconnect...", RED);
            server.disconnectPipe();
            server.PipeConnect();
            continue;
        }
        else if (response == 1)
        {
            cout << "Shutting Down...\n";
            break;
        }

        server.FlushBuffer();
        response = server.ReadFromPipe();
        if (response != 0)
        {
            if (response == -1)
                cout << "Client has shutdown!\n";
            else
                cout << "Error in client!\n";
            ShowMessage("Trying to Reconnect...\n", RED);
            server.disconnectPipe();
            server.PipeConnect();
        }
    }

    server.disconnectPipe();

    delete &server;

    return 1;
}