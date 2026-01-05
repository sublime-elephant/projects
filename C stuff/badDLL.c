#include <stdio.h>
#include <windows.h>;

#pragma comment(lib, "User32.lib");

__declspec(dllexport) int Hahaha();


int Hahaha() {
    printf("Wrong function \n");
    MessageBoxW(NULL, "bruh", NULL, MB_OK);
    return 0;
}

BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,  // handle to DLL module
    DWORD fdwReason,     // reason for calling function
    LPVOID lpvReserved )  // reserved
{
    // Perform actions based on the reason for calling.
    switch( fdwReason ) 
    { 
        case DLL_PROCESS_ATTACH:
            printf("Hi!\n");
            printf("This is a print statement from an injected DLL \n");
            MessageBoxW(NULL, L"bruh", NULL, MB_OK);
            break;

        case DLL_THREAD_ATTACH:
            printf("Thread attach detected\n");
            break;

        case DLL_THREAD_DETACH:
            printf("Thread detach detected\n");
            break;

        case DLL_PROCESS_DETACH:
        
            if (lpvReserved != NULL)
            {
                break; // do not do cleanup if process termination scenario
            }
            
         // Perform any necessary cleanup.
            break;
    }
    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}