class Data
{
public:
    string info;
};

class CustomNamedPipeClient
{

    HANDLE customPipe;

    LPTSTR pipeName;

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
    int OpenPipe(LPTSTR pName)
    {
        pipeName = pName;

        customPipe = CreateFile(
            pipeName,
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL);

        if (customPipe == INVALID_HANDLE_VALUE)
        {
            ShowMessage("Pipe Connection Attempt Failed!\n", RED);
            return -1;
        }
        else
        {
            ShowMessage("Pipe Connection Success.\n", GREEN);
            return 0;
        }
    }

    void OpenServerConnection(LPTSTR pName)
    {
        while (OpenPipe(pName) != FALSE)
        {
            if (GetLastError() == ERROR_FILE_NOT_FOUND)
                ShowMessage("Waiting for Server to Create Pipe...\n\n", YELLOW);
            Sleep(5000);
        }
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
};
