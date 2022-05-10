#include <iostream>
#include <Windows.h>

using namespace std;

#include "util.h"
#include "custom-named-pipe-server.h"

int main()
{

    cout << "---- SERVER ----\n\n";
    CustomNamedPipeServer server;

    server.PipeConnect();

    while (1)
    {
        char message[1023];
        cout << "Enter Message : ";
        cin.getline(message, 1023);

        server.WriteToPipe(message);

        server.FlushBuffer();

        server.ReadFromPipe();
    }

    server.disconnectPipe();

    delete &server;

    return 0;
}