class CustomNamedPipeServer
{

    HANDLE customPipe;
    LPTSTR pipeName = TEXT("\\\\.\\pipe\\mypipe");
    char inputBuffer[1023];
    char outputBuffer[1023];
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
    CustomNamedPipeServer()
    {
        customPipe = CreateNamedPipe(
            pipeName,
            PIPE_ACCESS_DUPLEX,
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            PIPE_UNLIMITED_INSTANCES,
            inputBufferSize,
            outputBufferSize,
            0,
            NULL);

        if (customPipe == INVALID_HANDLE_VALUE)
            ShowMessage("Error in Pipe Creation!\n", RED);
        else
            ShowMessage("\nPipe Creation Successful.\n", GREEN);
    }

    ~CustomNamedPipeServer()
    {
        CloseHandle(customPipe);
    }

    int PipeConnect()
    {
        bConnectNamedPipe = ConnectNamedPipe(customPipe, NULL);

        if (bConnectNamedPipe == FALSE)
        {
            ShowMessage("Error connection to the pipe!\n", RED);
            return -1;
        }
        else
        {
            ShowMessage("Connection successful.\n", GREEN);
            return 0;
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

        if (bWriteFile == FALSE)
        {
            ShowMessage("Error : Message not sent!\n", RED);
            return -1;
        }
        else
        {
            ShowMessage("Message Sent.\n", GREEN);
            return 0;
        }
    }

    int FlushBuffer()
    {
        bFlushFileBuffer = FlushFileBuffers(customPipe);

        if (bFlushFileBuffer == FALSE)
        {
            ShowMessage("FlushFileBuffer failed!\n", RED);
            return -1;
        }
        else
        {
            ShowMessage("FlushFileBuffer Successful.\n", GREEN);
            return 0;
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

        if (bReadFile == FALSE)
        {
            ShowMessage("Error : Message not received!\n", RED);
            return -1;
        }
        else
        {
            ShowMessage("Message Received.\n", GREEN);
            cout << "CLIENT -> " << readFileBuffer << endl;
            return 0;
        }
    }

    void disconnectPipe()
    {
        DisconnectNamedPipe(customPipe);
    }
};
