#include <stdio.h>
#include <Windows.h>
#define UNICODE

#pragma comment(lib, "User32.lib")

__declspec(dllexport) int ComputerName();

//This was a project to explore DLLs and maybe I 
//eventually even got to DLL hijacking.
// This commmand line utility is cracked for finding out what functions
// a DLL exports. I was able to use it to investigate an export issue.
// dumpbin /exports <file>

int ComputerName() {
   char buf[MAX_COMPUTERNAME_LENGTH + 1];
   DWORD nSize = MAX_COMPUTERNAME_LENGTH + 1;
   int a = GetComputerName(&buf, &nSize);
   if(a == 0){
      DWORD err = GetLastError();
      MessageBox(NULL, err, NULL, MB_OK);
      return 1;
   }
   printf("Computer name: %s\n", buf);
   return 0;


}
