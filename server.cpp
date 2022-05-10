#include <iostream>
#include <Windows.h>

using namespace std;

#include "util.h"

int main()
{

    cout << "---- SERVER ----\n\n";

    // Local Variables for named pipe

    HANDLE hCreateNamedPipe;
    LPTSTR lpszPipename = TEXT("\\\\.\\pipe\\mypipe");
    char szInputBuffer[1023];
    char szOutputBuffer[1023];
    DWORD dwszInputBuffer = sizeof(dwszInputBuffer);
    DWORD dwszOutputBuffer = sizeof(dwszOutputBuffer);

    // bConnect Variables
    BOOL bConnectNamedPipe;

    // Write File Variables
    BOOL bWriteFile;
    char szWriteFileBuffer[1023];
    DWORD dwWriteBufferSize = sizeof(szWriteFileBuffer);
    DWORD dwNoByteWrite;

    // Flush Buffer
    BOOL bFlushFileBuffer;

    // Read File Variables
    BOOL bReadFile;
    char szReadFileBuffer[1023];
    DWORD dwReadBufferSize = sizeof(szReadFileBuffer);
    DWORD dwNoByteRead;

    // Create Named Pipe
    hCreateNamedPipe = CreateNamedPipe(
        lpszPipename,
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES,
        dwszInputBuffer,
        dwszOutputBuffer,
        0,
        NULL);

    if (hCreateNamedPipe == INVALID_HANDLE_VALUE)
        ShowMessage("Error in Pipe Creation!\n", RED);
    else
        ShowMessage("\nPipe Creation Successful.\n", GREEN);

    // Connection to the pipe

    bConnectNamedPipe = ConnectNamedPipe(hCreateNamedPipe, NULL);

    if (bConnectNamedPipe == FALSE)
        ShowMessage("Error connection to the pipe!\n", RED);
    else
        ShowMessage("Connection successful.\n", GREEN);

    while (1)
    {
        cout << "Enter Message : ";
        cin.getline(szWriteFileBuffer, 1023);

        bWriteFile = WriteFile(
            hCreateNamedPipe,
            szWriteFileBuffer,
            dwWriteBufferSize,
            &dwNoByteWrite,
            NULL);

        if (bWriteFile == FALSE)
            ShowMessage("Error : Message not sent!\n", RED);
        else
            ShowMessage("Message Sent.\n", GREEN);

        // Flush the buffer
        bFlushFileBuffer = FlushFileBuffers(hCreateNamedPipe);

        if (bFlushFileBuffer == FALSE)
            ShowMessage("FlushFileBuffer failed!\n", RED);
        else
            ShowMessage("FlushFileBuffer Successful.\n", GREEN);

        // Read from File

        bReadFile = ReadFile(
            hCreateNamedPipe,
            szReadFileBuffer,
            dwReadBufferSize,
            &dwNoByteRead,
            NULL);

        if (bReadFile == FALSE)
            ShowMessage("Error : Message not received!\n", RED);
        else
            ShowMessage("Message Received.\n", GREEN);

        cout << "CLIENT -> " << szReadFileBuffer << endl;
    }

    DisconnectNamedPipe(hCreateNamedPipe);

    CloseHandle(hCreateNamedPipe);

    return 0;
}