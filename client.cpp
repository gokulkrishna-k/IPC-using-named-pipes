#include <windows.h>
#include <iostream>

using namespace std;

#include "util.h"
#include "custom-named-pipe-client.h"

int main()
{
    cout << "---- CLIENT ----\n\n";

    CustomNamedPipeClient client;

    LPTSTR pipeName = TEXT("\\\\.\\pipe\\mypipe");

    client.OpenServerConnection(pipeName);

    int response;
    while (1)
    {
        response = client.ReadFromPipe();
        if (response != 0)
        {
            if (response == 1)
                cout << "Server the shutdown!\n";
            cout << "Error in accessing server!\n";
            client.OpenServerConnection(pipeName);
            continue;
        }
        else if (response == 1)
        {
            cout << "Server has Disconnected!\n";
            client.OpenServerConnection(pipeName);
            continue;
        }

        char message[1023];
        cout << "Enter Message : ";
        cin.getline(message, 1023);
        string temp(message);

        response = client.WriteToPipe(message);

        if (response == -1)
        {
            ShowMessage("Error , Trying to Reconnect...", RED);
            client.OpenServerConnection(pipeName);
            continue;
        }
        else if (response == 1)
        {
            cout << "Shutting Down...\n";
            break;
        }
    }

    delete &client;
    return 1;
}