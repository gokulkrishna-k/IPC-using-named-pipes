class Data
{
public:
    string info;
};

class CustomNamedPipeServer
{

    HANDLE customPipe;
    LPTSTR pipeName;
    char inputBuffer[1023];
    char outputBuffer;
    DWORD inputBufferSize = sizeof(inputBuffer);
    DWORD outputBufferSize = sizeof(outputBuffer);

    BOOL bConnectNamedPipe;

    // WriteFile
    BOOL bWriteFile;
    char writeFileBuffer[1023];
    DWORD writeFileBufferSize = sizeof(writeFileBuffer);
    DWORD noBytesToWrite;

    // Flush Buffer
    BOOL bFlushFileBuffer;

    // Read File Variables
    BOOL bReadFile;
    char readFileBuffer[1023];
    DWORD readFileBufferSize = sizeof(readFileBuffer);
    DWORD noBytesToRead;

public:
    CustomNamedPipeServer(LPTSTR pName, DWORD pAccessMode, DWORD pType, DWORD instance, DWORD timeout)
    {

        pipeName = pName;
        customPipe = CreateNamedPipe(
            pipeName,
            pAccessMode,
            pType,
            instance,
            inputBufferSize,
            outputBufferSize,
            timeout,
            NULL);

        if (customPipe == INVALID_HANDLE_VALUE)
            ShowMessage("Error in Pipe Creation!\n", RED);
        else
            ShowMessage("\nPipe Creation Successful.\n", GREEN);
    }

    ~CustomNamedPipeServer()
    {
        cout << "Destroying Pipe..\n";
        CloseHandle(customPipe);
    }

    void PipeConnect()
    {
        cout << "Waiting for client Connection...\n";
        bConnectNamedPipe = ConnectNamedPipe(customPipe, NULL);

        if (bConnectNamedPipe == FALSE)
        {
            ShowMessage("Error connection to the pipe!\n", RED);
        }
        else
        {
            ShowMessage("Connection successful.\n", GREEN);
        }
    }

    int WriteToPipe(char message[])
    {

        bWriteFile = WriteFile(
            customPipe,
            message,
            writeFileBufferSize,
            &noBytesToWrite,
            NULL);

        if (bWriteFile == FALSE && GetLastError() == ERROR_PIPE_NOT_CONNECTED)
        {
            ShowMessage("Error :Pipe Not Connected!\n", RED);
            return -1;
        }
        else if (bWriteFile == FALSE && GetLastError() == ERROR_PIPE_BUSY)
        {
            ShowMessage("Error : Pipe Busy!\n", RED);
            return -1;
        }
        else if (bWriteFile == FALSE && GetLastError() == ERROR_BROKEN_PIPE)
        {
            ShowMessage("Error : Broken Pipe\n", RED);
            return -1;
        }
        else
        {
            ShowMessage("Message Sent.\n", GREEN);
            string temp(message);
            if (temp == "ESC")
                return 1;
            return 0;
        }
    }

    void FlushBuffer()
    {
        bFlushFileBuffer = FlushFileBuffers(customPipe);

        if (bFlushFileBuffer == FALSE)
        {
            ShowMessage("FlushFileBuffer failed!\n", RED);
        }
        else
        {
            ShowMessage("FlushFileBuffer Successful.\n", GREEN);
        }
    }

    int ReadFromPipe()
    {

        bReadFile = ReadFile(
            customPipe,
            readFileBuffer,
            readFileBufferSize,
            &noBytesToRead,
            NULL);

        if (bReadFile == FALSE && GetLastError() == ERROR_PIPE_NOT_CONNECTED)
        {
            ShowMessage("Error :Pipe Not Connected!\n", RED);
            return -1;
        }
        else if (bReadFile == FALSE && GetLastError() == ERROR_PIPE_BUSY)
        {
            ShowMessage("Error : Pipe Busy!\n", RED);
            return -1;
        }
        else if (bReadFile == FALSE && GetLastError() == ERROR_BROKEN_PIPE)
        {
            ShowMessage("Error : Broken Pipe\n", RED);
            return -1;
        }
        else
        {
            ShowMessage("Message Received.\n", GREEN);
            string temp(readFileBuffer);
            if (temp == "ESC")
                return 1;
            cout << "CLIENT -> " << temp << endl;
            return 0;
        }
    }

    void disconnectPipe()
    {
        cout << "Disconnecting Pipe...\n";
        DisconnectNamedPipe(customPipe);
    }
};

// void PipeListener()
// {
//     BOOL pipeState;

//     while (1)
//     {
//         pipeState = GetNamedPipeHandleState(
//             customPipe,
//             NULL, NULL, NULL, NULL, NULL, NULL);

//         if (GetLastError() == ERROR_PIPE_NOT_CONNECTED)
//         {
//             cout << "Client has disconnected!\n";
//         }
//         cout << "CLIENT OK\n";
//         Sleep(3000);
//     }
// }