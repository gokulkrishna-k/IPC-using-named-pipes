class CustomNamedPipeClient
{

    HANDLE customPipe;

    LPTSTR pipeName = TEXT("\\\\.\\pipe\\mypipe");

    // WriteFile
    BOOL bWriteFile;
    char writeFileBuffer[1023];
    DWORD writeFileBufferSize = sizeof(writeFileBuffer);
    DWORD noBytesToWrite;

    // Read File Variables
    BOOL bReadFile;
    char readFileBuffer[1023];
    DWORD readFileBufferSize = sizeof(readFileBuffer);
    DWORD noBytesToRead;

public:
    CustomNamedPipeClient()
    {
        customPipe = CreateFile(
            pipeName,
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL);

        if (customPipe == INVALID_HANDLE_VALUE)
            ShowMessage("File Creation Failed!\n", RED);
        else
            ShowMessage("File Creation Success.\n", GREEN);
    }

    ~CustomNamedPipeClient()
    {
        CloseHandle(customPipe);
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
};
