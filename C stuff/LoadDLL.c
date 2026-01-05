#include <stdio.h>
#include <Windows.h>
#define UNICODE
#pragma comment(lib, "User32.lib")

//Compiler notes  
/*
    /LD makes DLL
    /MT  static link the runtime.
    /MD  dynamic link the runtime.
*/

int main() {
    HMODULE hDLL = LoadLibrary("computername.dll");
    if (hDLL == NULL) {
        DWORD err = GetLastError();
        printf("hDLL failed: %lu\n", err);
        return 1;
    }
    FARPROC fp;
    fp = GetProcAddress(hDLL, "ComputerName");
    if (fp == 0) {
        DWORD err;
        err = GetLastError();
        char buf[50];
        snprintf(&buf, 50, "Error: %lu\n", err);
        printf(buf);
        printf("fp: %p", fp);
        return 1;
    }
    printf("Attempting call\n");
    fp();
    for (int i = 0; i < 100; i++){
        Sleep(1000);
        printf("Zzzz...\n");
        fp();
    }


}