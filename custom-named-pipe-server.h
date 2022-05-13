class CustomNamedPipeServer
{

    HANDLE customPipe;
    LPTSTR pipeName;

    DWORD inputBufferSize = 1023;
    DWORD outputBufferSize = 1023;

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
        while (1)
        {
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
            {
                ShowMessage("\nPipe Creation Successful.\n", GREEN);
                break;
            }
        }
    }

    ~CustomNamedPipeServer()
    {
        CloseHandle(customPipe);
    }

    DWORD PipeConnect()
    {
        cout << "Waiting for client Connection...\n";

        while (1)
        {
            bConnectNamedPipe = ConnectNamedPipe(customPipe, NULL);

            if (bConnectNamedPipe == FALSE && GetLastError() == ERROR_PIPE_LISTENING)
            {
                ShowMessage("Pipe Connect : Waiting for process to open other end of pipe!\n", RED);
            }
            else if (bConnectNamedPipe == FALSE && GetLastError() == ERROR_PIPE_CONNECTED)
            {
                ShowMessage("Pipe Connect : ERROR_PIPE_CONNECTED\n", RED);
            }
            else if (bConnectNamedPipe == TRUE)
            {
                ShowMessage("Pipe Connect : Connection successful.\n", GREEN);
                return TRUE;
            }
            Sleep(1000);
        }
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
                    return 1;
                return 0;
            }
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

    void disconnectPipe()
    {
        DisconnectNamedPipe(customPipe);
    }
};
