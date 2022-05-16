
#include "custom-named-pipe.h"
class CustomNamedPipeServer : public CustomNamedPipe
{

    LPTSTR pipeName;

    DWORD inputBufferSize = 1023;
    DWORD outputBufferSize = 1023;

    BOOL bConnectNamedPipe;

    // Flush Buffer
    BOOL bFlushFileBuffer;

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
