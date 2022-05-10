#include <Windows.h>
#include <iostream>

using namespace std;

#include "util.h"
#include "custom-named-pipe-client.h"

int main()
{
    cout << "---- CLIENT ----\n\n";

    CustomNamedPipeClient client;

    while (1)
    { // Read from the pipe
        client.ReadFromPipe();
        char message[1023];
        cout << "Enter Message : ";
        cin.getline(message, 1023);
        client.WriteToPipe(message);
    }

    delete &client;
    return 0;
}