#include <stdio.h>
#include <windows.h>
#include <memoryapi.h>
#include <stdlib.h>
#include <Psapi.h>
#include <accctrl.h>
#include <aclapi.h>

// The virtual address space for a process is the set of virtual memory addresses that it can use. The address space for each process is private and cannot be accessed by other processes unless it is shared.

//A virtual address does not represent the actual physical location of an object in memory; instead, the system maintains a page table for each process, which is an internal data structure used to translate virtual addresses into their corresponding physical addresses.Each time a thread references an address, the system translates the virtual address to a physical address.

//a "Paging file" is the act of expanding RAM memory by using the Hard disk. When extra space is needed/when certain information in RAM isn't required right now/something like that, the memory is "paged into a paging file" from RAM to the disk in a pagefile. This is what you see in Task manager under Performance -> Memory -> Committed header. As of writing, my committed header shows 11.5/27.9GB. Which means 11.5 GB of a maximum 27.9GB information is being paged on the hard-drive right now.

// "Working set" of a process is the set of pages in the virtual address space of the process that is currently resident in physical memory i.e., pages of memory not in a pagefile but in RAM.

// GetProcessMemoryInfo is likely a useful function. Also "PSAPI". VirtualAlloc, VirtualAllocEx, VirtualFree, VirtualFreeEx, GlobalAlloc, LocalAlloc.

//File mapping. Shared memory.

//"If your application must run code from a memory page, it must allocate and set the proper virtual memory protection attributes".

//The allocated memory must be marked PAGE_EXECUTE, PAGE_EXECUTE_READ, PAGE_EXECUTE_READWRITE, or PAGE_EXECUTE_WRITECOPY when allocating memory. Heap allocations made by calling the malloc and HeapAlloc functions are non-executable.

// What does Applications cannot run code from the default process heap or the stack mean?

//GetSystemDEPPolicy, SetProcessDEPPolicy.

//Copy-on-write protection is a way that processes can share a loaded DLL's (or other?) pages, but the moment a process writes to the page, it copies it into a new page (so as to not corrupt the page another process could be using).

//RESERVING a range (or page) in a process's virtual address space only prevents other functions from using the specified range.

// Each process has a default heap provided by the system. Applications that make frequent allocations from the heap can improve performance by using private heaps.

//A private heap is a block of one or more pages in the address space of the calling process. After creating the private heap, the process uses functions such as HeapAlloc and HeapFree to manage the memory in that heap.

//The heap functions can also be used to manage memory in the process's default heap, using the handle returned by the GetProcessHeap function. New applications should use the heap functions instead of the global and local functions for this purpose.

//There is no difference between memory allocated from a private heap and that allocated by using the other memory allocation functions.For a complete list of functions, see the table in Memory Management Functions.

//GetSystemInfo

//VirtualQuery, VirtualQueryEx


void systeminfo() {
	//Windows Docs specified that a parameter to GetSystemInfo was a SYSTEM_INFO structure. Here I literally create it!
	//the function needs the pointer to the structure, so &.
	SYSTEM_INFO SysInfoStructure;
	GetSystemInfo(&SysInfoStructure);
	printf("Page Size: %u\n", SysInfoStructure.dwPageSize);
	printf("Lowest memory address accessible by applications and DLLs: %p\n", SysInfoStructure.lpMinimumApplicationAddress);
	printf("Highest memory address accessible by applications and DLLs: %p\n", SysInfoStructure.lpMaximumApplicationAddress);
	printf("Active processor mask: %p\n", (void*)SysInfoStructure.dwActiveProcessorMask);
	printf("Number of processors: %u\n", SysInfoStructure.dwNumberOfProcessors);
	printf("Processor type: %u\n", SysInfoStructure.dwProcessorType);
	printf("Allocation granularity: %u\n", SysInfoStructure.dwAllocationGranularity);
	printf("Processor level: %u\n", SysInfoStructure.wProcessorLevel);
	printf("Processor reivision: %u\n", SysInfoStructure.wProcessorRevision);
}

LPVOID MaximumMemory() {
	SYSTEM_INFO SysInfoStructure;
	GetSystemInfo(&SysInfoStructure);
	return SysInfoStructure.lpMaximumApplicationAddress;
}

int SearchMemory(HANDLE hProcess, void* pAddress, int iSize, int *aFind, int aFindSize) {
	// Errors

	char* pBuffer = malloc(iSize);

	if (!pBuffer) {
		printf("malloc went wrong %u", GetLastError());
		return 1;
	}
	if (!ReadProcessMemory(hProcess, pAddress, pBuffer, iSize, NULL) && GetLastError() != 299) {
		printf("Error reading process memory: %u", GetLastError());
		return 1;
	}

	for (int i = 0; i < iSize; i++) {
		for (int a = 0; a < aFindSize; a++) {
			if (pBuffer[i + a] != aFind[a]) {
				break;
			}
			if (a == aFindSize - 1) {
				printf("Last one, it's probably a match\n\n");
				for (int b = 0; b < aFindSize; b++) {
					printf("%c", pBuffer[i + b]);
				}
				printf("\nmatched\n");
				for (int c = 0; c < aFindSize; c++) {
					printf("%c", aFind[c]);
				}
				printf("\nat %p\n\n", (unsigned long long)pAddress + i);
				
			}
		}
	}
}


int ReadMemory(HANDLE hProcess, void* pAddress, int iSize) {

	// We allocate a buffer in size bytes, given to us from the RegionSize parameter in the VirtualQueryEx call in ScanOtherMemory

	char* pBuffer = malloc(iSize);

	// Errors 

	if (!pBuffer) {
		printf("malloc went wrong %u", GetLastError());
		return 1;
	}
	if (!ReadProcessMemory(hProcess, pAddress, pBuffer, iSize, NULL) && GetLastError() != 299) {
		printf("Error reading process memory: %u", GetLastError());
		return 1;
	}

	//Formatting the layout. This code works by iterating char by char through the buffer, and printing the hex value if it's within ASCII range. When the 16th byte is reached, it tabs over to start the ASCII representation of the data. 


	printf("\t----------------------------------------------------------------\n");
	printf("\t");
	for (int i = 0; i < iSize ; i++) {
		if (pBuffer[i] > 31 && pBuffer[i] < 128) {
			if (i % 16 == 0 && i != 0) {
				printf("\t");
				for (int z = 0; z < 16; z++) {
					if (pBuffer[i - (16 - z)] > 31 && pBuffer[i - (16 - z)] < 128) { //Goes back in the buffer to the start of the line, and reprints the chars in char format
						printf("%c", pBuffer[i - (16 - z)]);
					}
					else { printf("."); }
				}
				printf("\n\t");
				
			}
			//Print hex of ASCII
			printf("%02x", pBuffer[i]);
		}
		else {
			if (i % 16 == 0 && i != 0) {
				//ASCII Representation
				printf("\t");
				for (int z = 0; z < 16; z++) {
					if (pBuffer[i - (16 - z)] > 31 && pBuffer[i - (16 - z)] < 128) {
						printf("%c", pBuffer[i - (16 - z)]);
					}
					else { printf("."); }
				}
				printf("\n\t");
			}
			//Not ASCII
			printf("..");
		}
	}
	printf("\n\t----------------------------------------------------------------\n\n\n");
	free(pBuffer);
	return 0;
}

int ScanOtherMemory(HANDLE hProcess, void* pAddress, int *aFind, int aFindSize) {

	// Variable that will be filled by VirtualQueryEx

	MEMORY_BASIC_INFORMATION mbi;

	//Error checks

	if (!VirtualQueryEx(hProcess, pAddress, &mbi, sizeof(mbi)) && GetLastError() == ERROR_INVALID_PARAMETER) {
		printf("Max recursion depth in process memory achieved. %u should be 87\n", GetLastError());
		return 1;
	}
	if (!VirtualQueryEx(hProcess, pAddress, &mbi, sizeof(mbi))) {
		printf("Generic error %u\n", GetLastError());
		return 1;
	}

	// Calculate next address for recursive VirtualQueryEx call to traverse

	void* nextAddress = (void*)((unsigned long long)mbi.BaseAddress + mbi.RegionSize);

	//Interact only with the Committed memory regions, calling ReadMemory to explore the contiguous committed memory region.
	//ReadMemory takes the process handle, address to start from, and the bytes to read

	if (mbi.State == MEM_COMMIT) {
		switch (mbi.AllocationProtect) {
		case 0x20:
			printf("Memory region: PAGE_EXECUTE_READ ");
			printf("allocation start: %p \n\n\t%p to %p\n", mbi.AllocationBase, mbi.BaseAddress, nextAddress);
			SearchMemory(hProcess, pAddress, mbi.RegionSize, aFind, aFindSize);
			break;
		case 0x40:
			printf("Memory region: PAGE_EXECUTE_READWRITE ");
			printf("allocation start: %p \n\n\t%p to %p\n", mbi.AllocationBase, mbi.BaseAddress, nextAddress);
			SearchMemory(hProcess, pAddress, mbi.RegionSize, aFind, aFindSize);
			break;
		case 0x80:
			printf("Memory region: PAGE_EXECUTE_WRITECOPY ");
			printf("allocation start: %p \n\n\t%p to %p\n", mbi.AllocationBase, mbi.BaseAddress, nextAddress);
			SearchMemory(hProcess, pAddress, mbi.RegionSize, aFind, aFindSize);
			break;
		case 0x02:
			printf("Memory region: PAGE_READONLY ");
			printf("allocation start: %p \n\n\t%p to %p\n", mbi.AllocationBase, mbi.BaseAddress, nextAddress);
			SearchMemory(hProcess, pAddress, mbi.RegionSize, aFind, aFindSize);
			break;
		case 0x04:
			printf("Memory region: PAGE_READWRITE ");
			printf("allocation start: %p \n\n\t%p to %p\n", mbi.AllocationBase, mbi.BaseAddress, nextAddress);
			SearchMemory(hProcess, pAddress, mbi.RegionSize, aFind, aFindSize);
			break;
		case 0x08:
			printf("Memory region: PAGE_WRITECOPY ");
			printf("allocation start: %p \n\n\t%p to %p\n", mbi.AllocationBase, mbi.BaseAddress, nextAddress);
			SearchMemory(hProcess, pAddress, mbi.RegionSize, aFind, aFindSize);
			break;
		default:
			printf("Memory region likely NON-READABLE ");
			printf("allocation start: %p \n\n\t%p to %p\n", mbi.AllocationBase, mbi.BaseAddress, nextAddress);
			SearchMemory(hProcess, pAddress, mbi.RegionSize, aFind, aFindSize);
			break;
		}
	}

	// Recursive call with start address as the calculated end of the previous block

	ScanOtherMemory(hProcess, nextAddress, aFind, aFindSize);
}

MEMORY_BASIC_INFORMATION MemoryScan(uintptr_t iAddress) {
	void* pStartAddress = (void*)iAddress;
	MEMORY_BASIC_INFORMATION mbiOutput;
	VirtualQuery(pStartAddress, &mbiOutput, sizeof(mbiOutput));
	printf("Base Address: %p\n", mbiOutput.BaseAddress);
	printf("Allocation Base: %p\n", mbiOutput.AllocationBase);
	printf("This memory block is: %llu bytes long.\n", (unsigned long long)mbiOutput.RegionSize);
	/*printf("Protection option: %u\n", mbiOutput.AllocationProtect);*/
	//printf("State: %x\n", mbiOutput.State);
	//printf("Protection state: %x\n", mbiOutput.Protect);
	//printf("Type of pages in the region: %x\n", mbiOutput.Type);
	switch (mbiOutput.State) {
		case 0x1000:
			printf("Memory region %p is COMMITTED\n", mbiOutput.BaseAddress);
			break;
		case 0x10000:
			printf("Memory region %p is FREE\n", mbiOutput.BaseAddress);
			break;
		case 0x2000:
			printf("Memory region %p is RESERVED\n", mbiOutput.BaseAddress);
			break;
	}
	if (mbiOutput.State == 0x1000) {
		switch (mbiOutput.Type) {
		case 0x1000000:
			printf("Mapped into the view of an image section.\n");
			break;
		case 0x40000:
			printf("mapped into the view of a section.\n");
			break;
		case 0x20000:
			printf("Private.\n");
			break;
		default:
			printf("idk\n");
			break;
		}
	}
	return mbiOutput;
}

MEMORY_BASIC_INFORMATION ScanCommittedOnly(uintptr_t ptrAddress, uintptr_t MaxAddress) {
	void* pStartAddress = ptrAddress;
	MEMORY_BASIC_INFORMATION mbiOutput;
	VirtualQuery(pStartAddress, &mbiOutput, sizeof(mbiOutput));
	if (mbiOutput.State == 0x1000) {
		uintptr_t uiptrStartAddress = (uintptr_t)mbiOutput.AllocationBase;
		uintptr_t uiptrEndAddress = uiptrStartAddress + mbiOutput.RegionSize;
		int dwRegionSize = mbiOutput.RegionSize;
		char* ptrCursor = (char*)mbiOutput.BaseAddress;
		printf("\n(%.4f%%) ", ((long double)uiptrStartAddress / (long double)MaxAddress) * 100);
		printf("%llx to %llx, size %zu\n", (void*)uiptrStartAddress, (void*)uiptrEndAddress, mbiOutput.RegionSize);
		printf("%p / %p\n", (void*)uiptrStartAddress, MaximumMemory());
		/*switch (mbiOutput.Type) {
		case 0x1000000:
			printf("Mapped into the view of an image section.\n");
			break;
		case 0x40000:
			printf("Mapped into the view of a section.\n");
			break;
		case 0x20000:
			printf("Private.\n");
			break;
		}*/
		switch (mbiOutput.Protect) {
		case 0x10:
			printf("Protection state is PAGE_EXECUTE\n");
			break;
		case 0x20:
			printf("Protection state is PAGE_EXECUTE_READ\n");
			vDereference(ptrCursor, dwRegionSize);
			break;
		case 0x40:
			printf("Protection state is PAGE_EXECUTE_READWRITE\n");
			vDereference(ptrCursor, dwRegionSize);
			break;
		case 0x80:
			printf("Protection state is PAGE_EXECUTEWRITECOPY\n");
			vDereference(ptrCursor, dwRegionSize);
			break;
		case 0x01:
			printf("Protection state is PAGE_NOACCESS\n");
			break;
		case 0x02:
			printf("Protection state is PAGE_READONLY\n");
			vDereference(ptrCursor, dwRegionSize);
			break;
		case 0x04:
			printf("Protection state is PAGE_READWRITE\n");
			vDereference(ptrCursor, dwRegionSize);
			break;
		case 0x08:
			printf("Protection state is PAGE_WRITECOPY\n");
			vDereference(ptrCursor, dwRegionSize);
			break;
		case 0x40000000:
			printf("Protection state is PAGE_TARGETS_INVALID\n");
			break;
		case 0:
			printf("Protection state is \"Caller has no access\"\n");
			break;
		}
	}
	return mbiOutput;
}

int Recurser(uintptr_t uiptrAddress, uintptr_t MaxAddress) {
	if (uiptrAddress <= MaxAddress) {
		
		MEMORY_BASIC_INFORMATION current_scan = ScanCommittedOnly(uiptrAddress, MaxAddress);
		uintptr_t uiptrNextAddress = (unsigned long long)current_scan.BaseAddress + current_scan.RegionSize;
		
		Recurser(uiptrNextAddress, MaxAddress);
	}
	else {
		printf("Recursion end reached\n");
		return 0;
	}
}

int vDereference(char* ptrCursor, int size) {
	printf("attempting to dereference %p\n", ptrCursor);
	char* pb = ptrCursor;
	for (int i = 0; i < size; i++) {
		if (0 < *ptrCursor && *ptrCursor < 127) {
			printf("%c", *ptrCursor);
		}
		else {
			printf(".");
		}
		ptrCursor = pb + i;
	}
	return 0;
}

int main() {
	/*void* ptr = VirtualAlloc(NULL, 8192, MEM_RESERVE, PAGE_READONLY);
	if (ptr == NULL) {
		printf("failed");
	}
	else { printf("succeeded: %p \n", ptr); }

	FILE* fp = NULL;
	errno_t err;
	if ((err = freopen_s(&fp, "output.txt", "w", stdout)) != 0) {
		printf("nope, %d", err);
	}
	else {
		uintptr_t uiptrMaxMemory = MaximumMemory();

		char string[] = "CUSTOM STRING";
		char* pString = &string;
		printf("address of string: %p", pString);

		Recurser(0, uiptrMaxMemory);

		fclose(fp);
	}*/

	/*int j = 1;
	char a[] = "teststring";
	int* p = &j;
	char* b = &j;
	printf("j is %i, at %p, same thing:%p, dereferenced: %d\nchar a is at: %p\n", j, &j, p, *p, &a);
	int* p2 = p + 1;
	printf("pointer arithmetic: p is %p, p2 is %p\ndereferencing p2 %c\n", p, p2, *p2);
	for (int i = 0; i < 256; i++) {
		char* p3 = b + i;
		if (0 < *p3 && *p3 < 127) {
			printf("%c", *p3);
		}
		else {
			printf(".");
		}
	}

	int* pf = (int*)0x7f7f7f;
	printf("\nattempting to read %p", *(int*)0x734747);*/

	HANDLE hSelfID = GetCurrentProcess();
	HANDLE hSelfToken = NULL;



	if (!(OpenProcessToken(hSelfID, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hSelfToken))) {

		printf("failed\n");

		return 1;

	}
	else {
		TOKEN_PRIVILEGES tp;
		LUID luid;
		char* privString = SE_DEBUG_NAME;

		if (!LookupPrivilegeValue(NULL, privString, &luid)) {

			printf("Lookup error: %u\n", GetLastError());

			return 1;
 
		}

		tp.PrivilegeCount = 1;
		tp.Privileges[0].Luid = luid;
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		if (!AdjustTokenPrivileges(hSelfToken, 0, &tp, sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES)NULL, (PDWORD)NULL)) {

			printf("Adjust error: %u\n", GetLastError());

			return 1;

		}

		if (GetLastError() == ERROR_NOT_ALL_ASSIGNED) {

			printf("the token does not have the privilege \n");

			return 1;

		}

		TOKEN_PRIVILEGES* tpPrivs;
		DWORD needed2 = 0;

		if (!GetTokenInformation(hSelfToken, 3, NULL, 0, &needed2)) {

			tpPrivs = malloc(needed2);

			if (tpPrivs == 0) {

				printf("malloc failed");

			}
			else {

				if (!GetTokenInformation(hSelfToken, 3, tpPrivs, needed2, &needed2)) {

					printf("error: %d\n", GetLastError());

					return 1;

				}
				else {

					DWORD dwPrivilegeCount = tpPrivs->PrivilegeCount;

					for (int i = 0; i < dwPrivilegeCount; i++) {

						LUID luidLUID = tpPrivs->Privileges[i].Luid;
						DWORD dwAttributes = tpPrivs->Privileges[i].Attributes;
						char* cLUID;
						DWORD tcBuffer = 0;

						//tcBuffer is filled with a TCHAR size, but malloc takes a size in bytes. I can't really tell
						//what went wrong in my mind, I thought char was 1 bit, or TCHAR was 4 bits, or something like that.

						if (!LookupPrivilegeName(NULL, &luidLUID, 0, &tcBuffer)) {

							cLUID = malloc(tcBuffer * sizeof(TCHAR));

							if (LookupPrivilegeName(NULL, &luidLUID, cLUID, &tcBuffer)) {

								printf("%lls:%lu\n", cLUID, dwAttributes);

							}
							else {

								printf("failed\n");

							}
							free(cLUID);
						}
					}

				}
				free(tpPrivs);
			}
		}
	}




	DWORD pPIDs[250];
	DWORD needed;

	if (!EnumProcesses(pPIDs, sizeof(pPIDs), &needed)) {

		printf("failed\n");

	}

	else {

		printf("pPIDs was %lu bytes, needed was %lu bytes, difference was %u\n", sizeof(pPIDs), needed, sizeof(pPIDs) - needed);

		for (int i = 0; i < needed / sizeof(pPIDs[1]); i++) {

			char lpFileName[200];
			char lpFileName2[200];

			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pPIDs[i]);


			//ERROR checking

			if (!hProcess && GetLastError() == ERROR_INVALID_PARAMETER) {

				printf("Skipping SYSTEMIDLEPROCESS\n\n");

				continue;

			}
			if (!hProcess && GetLastError() == ERROR_ACCESS_DENIED) {

				printf("Access denied\n\n");

				continue;

			}

			if (!hProcess) {

				printf("Generic error: %u", GetLastError());

				return 1;

			}


			//Process .exe title and then full path.

			if (!GetModuleBaseNameA(hProcess, NULL, lpFileName2, sizeof(lpFileName2))) {

				printf("failed, %u\n", GetLastError());

				return 1;

			}

			printf("%s : %u\n\n", lpFileName2, pPIDs[i]);

			if (!GetModuleFileNameExA(hProcess, NULL, lpFileName, sizeof(lpFileName))) {

				printf("failed, %u\n", GetLastError());

				return 1;

			}
			printf("%s\n\n", lpFileName);

			printf("-----------\n");


		}

		printf("Process count: %i\n", (needed / sizeof(pPIDs[1])));

	}



	//HILARIOUS old code below
	/*else {
		if ((sizeof(pPIDs)/sizeof(DWORD*)) >= needed) {
			printf("buffer size all good (difference: %llu)\n", ((sizeof(pPIDs)/sizeof(DWORD*)) - needed));
			for (int i = 0; i < (needed/sizeof(DWORD*)); i++) {
				HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, 0, pPIDs[i]);
				if (!hProcess && GetLastError() == ERROR_INVALID_PARAMETER) {
					printf("failed opening process, invalid parameter (likely trying to open SystemIdleProcess 0x00)\n");
					continue;
				}
				if (!hProcess && GetLastError() == ERROR_ACCESS_DENIED) {
					printf("failed opening process, access denied\n");
					continue;
				}
				else {
					printf("Successful");
					LPSTR* lpString[30];
					DWORD dwProcessName = GetProcessImageFileNameA(hProcess, lpString, sizeof(lpString));
					if (!dwProcessName) {
						printf("failed grabbing name \n");
						return 0;
					}
					else {
						printf("%s", *lpString);
					}
				}
			}
			printf("process count: %llu", (needed/sizeof(needed)));
		}
		else {
			printf("buffer size too small");
			return 0;
		}*/
	int aFind[] = { 0x41 };
	int aFindSize = sizeof(aFind) / sizeof(aFind[0]);
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, 15104);
	if (!ScanOtherMemory(hProcess, 0x00, &aFind, aFindSize)) {
		printf("Error\n");
		return 1;
	}
	else { printf("success\n"); }

	return 0;

}
