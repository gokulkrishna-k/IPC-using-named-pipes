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
        for (int i = 0; i < RETRY_LIMIT; i++)
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

        OVERLAPPED ol = {0, 0, 0, 0, NULL};
        BOOL ret = 0;

        ol.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

        ret = ConnectNamedPipe(customPipe, &ol);

        if (ret == FALSE)
        {

            switch (GetLastError())
            {
            case ERROR_PIPE_CONNECTED:
            {
                ret = TRUE;
                ShowMessage("Pipe Connect : Already Connected.\n", GREEN);
                break;
            }

            case ERROR_IO_PENDING:
            {
                cout << "CONNECTION PENDING\n";
                if (WaitForSingleObject(ol.hEvent, PIPE_TIMEOUT_CONNECT) == WAIT_OBJECT_0)
                {
                    DWORD dwIgnore;
                    ret = GetOverlappedResult(customPipe, &ol, &dwIgnore, FALSE);
                }
                else
                {
                    ShowMessage("Pipe Connect : CancelIO\n", YELLOW);
                    CancelIo(customPipe);
                }
                break;
            }
            }
        }
        if (ret == TRUE)
        {
            ShowMessage("Pipe Connect : Connection successful.\n", GREEN);
            CloseHandle(ol.hEvent);
            return TRUE;
        }
        if (ret == FALSE)
        {
            ShowMessage("Pipe Connect : Timeout\n", YELLOW);
            disconnectPipe();
            CloseHandle(customPipe);
            return FALSE;
        }
    }

    DWORD WriteToPipe(char message[])
    {

        for (int i = 0; i < RETRY_LIMIT; i++)
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
            else if (bWriteFile == TRUE)
            {
                string temp(message);
                if (temp == "ESC")
                    return 1;
                return 0;
            }
        }

        return -1;
    }

    DWORD ReadFromPipe()
    {

        for (int i = 0; i < RETRY_LIMIT; i++)
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
                Sleep(500);
                continue;
            }
            else if (bReadFile == FALSE && GetLastError() == ERROR_BROKEN_PIPE)
            {
                ShowMessage("Read Error : Broken Pipe\n", RED);
                return ERROR_BROKEN_PIPE;
            }
            else if (bReadFile == TRUE)
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
        cout << "Disconnected.\n";
        DisconnectNamedPipe(customPipe);
    }
};
