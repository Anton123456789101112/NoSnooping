#include <iostream>
#include <windows.h> 
#include <cstdlib> 
#include <conio.h> 
#include <fstream>
#include <string>

using namespace std; // Bad practice...

void hideWindow()
{
    HWND Stealth;
    Stealth = FindWindowA("ConsoleWindowClass", NULL);
    ShowWindow(Stealth,0);
}

void setAutostart(bool enable) 
{
    const char* regPath = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
    const char* valueName = "MyApp";

    if (enable) 
    {
        char exePath[MAX_PATH];
        DWORD len = GetModuleFileNameA(NULL, exePath, MAX_PATH);
        if (len == 0) 
            return;

        HKEY hKey;
        if (RegOpenKeyExA(HKEY_CURRENT_USER, regPath, 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) 
        {
            RegSetValueExA(hKey, valueName, 0, REG_SZ, (const BYTE*)exePath, (DWORD)(strlen(exePath) + 1));
            RegCloseKey(hKey);
        }
    } 
    else 
    {
        HKEY hKey;
        if (RegOpenKeyExA(HKEY_CURRENT_USER, regPath, 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) 
        {
            RegDeleteValueA(hKey, valueName);
            RegCloseKey(hKey);
        }
    }
}

DWORD getIdleTime() 
{
    LASTINPUTINFO lii;
    lii.cbSize = sizeof(LASTINPUTINFO);
    lii.dwTime = 0;

    if (GetLastInputInfo(&lii)) 
        return (GetTickCount() - lii.dwTime);
    
    return 0;
}

void logActivity(string s)
{
    ofstream log("log.txt", ios::app);
    log << s << ": " << GetTickCount() << '\n';
    log.close();
}


int main()
{
    string logShutdown = "Shutdown initiated";
    string logSnooping = "Snooping  detected";

    hideWindow();
    Sleep(1000);

    while (true)
    {
        SetCursorPos(990, 540);

        // Hinder ctrl or windows commands
        if (GetAsyncKeyState(VK_CONTROL) || GetAsyncKeyState(VK_LWIN) || GetAsyncKeyState(VK_RWIN))
        {
            logActivity(logShutdown);
            // Change 0 to 1 if the logging isn't fast enough
            system("shutdown -s -f -t 0");
        }
        // Secret code to stop
        else if ((GetAsyncKeyState(0x33)) && (GetAsyncKeyState(0x48)) && (GetAsyncKeyState(0x74))) // 3 + h + F5
        {
            setAutostart(false);
            return 0;
        }
        // If snooper is snooping, log it
        else if (getIdleTime() < 1)
            logActivity(logSnooping);
    }
}

