#include <Windows.h>
#include <iostream>

using namespace std;

#include "util.h"

int main()
{
    cout << "---- CLIENT ----\n\n";

    HANDLE hCreateFile;
    LPTSTR lpszPipename = TEXT("\\\\.\\pipe\\mypipe");
    // ReadFile Variables

    BOOL bReadFile;
    char szReadFileBuffer[1023];
    DWORD dwszReadFileBufferSize = sizeof(szReadFileBuffer);
    DWORD dwNoBytesRead;

    // WriteFile  Variables

    BOOL bWriteFile;
    char szWriteFileBuffer[1023];
    DWORD dwszWriteFileBuffferSize = sizeof(szWriteFileBuffer);
    DWORD dwNoBytesWrite;

    // Create File

    hCreateFile = CreateFile(
        lpszPipename,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (hCreateFile == INVALID_HANDLE_VALUE)
        ShowMessage("File Creation Failed!\n", RED);
    else
        ShowMessage("File Creation Success.\n", GREEN);

    while (1)
    { // Read from the pipe
        bReadFile = ReadFile(
            hCreateFile,
            szReadFileBuffer,
            dwszReadFileBufferSize,
            &dwNoBytesRead,
            NULL);

        if (bReadFile == FALSE)
            ShowMessage("Error : Message not received!\n", RED);
        else
            ShowMessage("Message Received.\n", GREEN);

        cout << "SERVER -> " << szReadFileBuffer << endl;

        cout << "Enter Message : ";
        cin.getline(szWriteFileBuffer, 1023);

        // Write to the pipe
        bWriteFile = WriteFile(
            hCreateFile,
            szWriteFileBuffer,
            dwszWriteFileBuffferSize,
            &dwNoBytesWrite,
            NULL);

        if (bWriteFile == FALSE)
            ShowMessage("Error : Message not sent!\n", RED);
        else
            ShowMessage("Message Sent.\n", GREEN);
    }
    // Close the pipe
    CloseHandle(hCreateFile);

    return 0;
}