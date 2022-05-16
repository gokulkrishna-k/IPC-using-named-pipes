class CustomNamedPipe
{
protected:
    HANDLE customPipe;
    // WriteFile
    BOOL bWriteFile;
    char writeFileBuffer[BUFF_SIZE];
    DWORD writeFileBufferSize = sizeof(writeFileBuffer);
    DWORD noBytesToWrite;

    // Read File Variables
    BOOL bReadFile;
    char readFileBuffer[BUFF_SIZE];
    DWORD readFileBufferSize = sizeof(readFileBuffer);
    DWORD noBytesToRead;

public:
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
                cout << "MESSAGE -> " << temp << endl;
                if (temp == "ESC")
                    return 1;
                return 0;
            }
        }
        return -1;
    }
};