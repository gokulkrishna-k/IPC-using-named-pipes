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
    int OpenPipe(LPTSTR pName, DWORD openMode)
    {
        pipeName = pName;
        customPipe = CreateFile(
            pipeName,
            openMode, 0,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL);
        cout << "OpenPipe : " << GetLastError() << endl;
        if (customPipe == INVALID_HANDLE_VALUE)
        {
            ShowMessage("Pipe Connection Attempt Failed!\n", RED);
            return FALSE;
        }
        else
        {

            ShowMessage("Pipe Connection Success.\n", GREEN);
            return TRUE;
        }
    }

    void OpenServerConnection(LPTSTR pName, DWORD openMode)
    {
        while (1)
        {
            BOOL res = OpenPipe(pName, openMode);

            if (res == TRUE)
                return;
            if (GetLastError() == ERROR_FILE_NOT_FOUND)
            {
                ShowMessage("Waiting for Server to Create Pipe...\n\n", YELLOW);
            }
            Sleep(1000);
        }
    }

    ~CustomNamedPipeClient()
    {
        cout << "Closing Pipe Handle...\n";
        CloseHandle(customPipe);
    }

    DWORD WriteToPipe(char message[])
    {

        while (1)
        {
            bWriteFile = WriteFile(
                customPipe,
                message,
                writeFileBufferSize,
                &noBytesToWrite,
                NULL);

            if (bWriteFile == FALSE && GetLastError() == ERROR_PIPE_NOT_CONNECTED)
            {
                ShowMessage("Write Error :Pipe Not Connected!\n", RED);
                return ERROR_PIPE_NOT_CONNECTED;
            }
            else if (bWriteFile == FALSE && GetLastError() == ERROR_PIPE_BUSY)
            {
                ShowMessage("Write Error : Pipe Busy!\n", RED);
                Sleep(1000);
                continue;
            }
            else if (bWriteFile == FALSE && GetLastError() == ERROR_BROKEN_PIPE)
            {
                ShowMessage("Write Error : Broken Pipe\n", RED);
                return ERROR_BROKEN_PIPE;
            }
            else
            {
                string temp(message);
                if (temp == "ESC")
                    return 1;
                return 0;
            }
        }
    }

    DWORD ReadFromPipe()
    {

        while (1)
        {
            bReadFile = ReadFile(
                customPipe,
                readFileBuffer,
                readFileBufferSize,
                &noBytesToRead,
                NULL);

            if (bReadFile == FALSE && GetLastError() == ERROR_PIPE_NOT_CONNECTED)
            {
                ShowMessage("Read Error :Pipe Not Connected!\n", RED);
                return ERROR_PIPE_NOT_CONNECTED;
            }
            else if (bReadFile == FALSE && GetLastError() == ERROR_PIPE_BUSY)
            {
                ShowMessage("Read Error : Pipe Busy!\n", RED);
                Sleep(1000);
                continue;
            }
            else if (bReadFile == FALSE && GetLastError() == ERROR_BROKEN_PIPE)
            {
                ShowMessage("Read Error : Broken Pipe\n", RED);
                return ERROR_BROKEN_PIPE;
            }
            else
            {

                string temp(readFileBuffer);
                cout << "CLIENT -> " << temp << endl;
                if (temp == "ESC")
                {
                    return 1;
                }
                return 0;
            }
        }
    }
};
