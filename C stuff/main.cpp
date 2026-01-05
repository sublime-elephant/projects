#include <stdio.h>
#include <windows.h>
#include <aclapi.h>

#define UNICODE

int wmain(int argc, WCHAR *argv[]) {
    wprintf(L"Trying to find: %ls\n", argv[1]);
    WIN32_FIND_DATAW w32findData;

    LPCWSTR wFileString = argv[1];
    HANDLE handleSearch = FindFirstFileW(wFileString, &w32findData);
    if (handleSearch == INVALID_HANDLE_VALUE){
        DWORD err = GetLastError();
        if (err == ERROR_FILE_NOT_FOUND) {
            wprintf(L"File not found \n");
            return 1;
        }
        wprintf(L"Error: %lu\n", err);
        return 1;
    }
    if (w32findData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY){
        wprintf(L"Directory: %ls\n", w32findData.cFileName);
    }
    if (w32findData.dwFileAttributes == FILE_ATTRIBUTE_ARCHIVE){
        wprintf(L"File: %ls\n", w32findData.cFileName);
    }
    wprintf(L"File: %ls\n", w32findData.cFileName);
    while (FindNextFileW(handleSearch, &w32findData) != 0) {
        if (w32findData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY){
            wprintf(L"Directory: %ls\n", w32findData.cFileName);
        }
        if (w32findData.dwFileAttributes == FILE_ATTRIBUTE_ARCHIVE){
            wprintf(L"File: %ls\n", w32findData.cFileName);
        }
    }
    FindClose(handleSearch);
}

void recurseFolder() {
    
}