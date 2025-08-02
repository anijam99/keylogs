# keylogs
A simple keylogger project written in C.

**Disclaimer**: This software is for educational and ethical purposes only.

## Features

- Basic keystroke logging
- Timestamp recording for each keystroke
- Window title tracking (knows which application had focus)
- Stealth operation (no visible console window)

## Building

1. Compile with a C compiler that supports Windows API (MinGW, Visual Studio)
2. Requires Windows operating system

## Usage

Simply run the compiled executable. Keystrokes will be logged to `keylog.txt` in the same directory.