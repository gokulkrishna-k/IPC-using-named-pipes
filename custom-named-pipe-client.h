#include "custom-named-pipe.h"

class CustomNamedPipeClient : public CustomNamedPipe
{

    LPTSTR pipeName;

public:
    BOOL OpenServerConnection(LPTSTR pName, DWORD openMode)
    {
        for (int i = 0; i < RETRY_LIMIT; i++)
        {
            pipeName = pName;
            customPipe = CreateFile(
                pipeName,
                openMode, 0,
                NULL,
                OPEN_EXISTING,
                FILE_ATTRIBUTE_NORMAL,
                NULL);
            if (customPipe == INVALID_HANDLE_VALUE)
            {
                ShowMessage("Pipe Connection Attempt Failed!\n", RED);
                if (GetLastError() == ERROR_FILE_NOT_FOUND)
                {
                    ShowMessage("Waiting for Server to Create Pipe...\n\n", YELLOW);
                }
            }
            else
            {
                ShowMessage("Pipe Connection Success.\n", GREEN);
                return TRUE;
            }
            Sleep(1000);
        }

        return FALSE;
    }

    ~CustomNamedPipeClient()
    {
        cout << "Closing Pipe Handle...\n";
        CloseHandle(customPipe);
    }
};
