#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <stdio.h>
#include <wchar.h>
#include <wininet.h>
#include <Winineti.h>
#include <ProofOfPossessionCookieInfo.h>
#include <errhandlingapi.h>
#include <locale.h>
#include <time.h>

int wmain() {


	/*int y = 888;
	int s[777];
	char* gg;
	gg = (char*)malloc(y);
	printf("size of s:%d\, size of gg:%d\n", sizeof(s) / sizeof(s[0]), sizeof(gg) / sizeof(gg[0]));
	gg[33] = 'k';
	gg[887] = '\0';
	gg[886] = 'i';
	gg[885] = 'h';
	printf("%s\n", gg);
	printf("size of s:%d\, size of gg:%d\n", sizeof(s) / sizeof(s[0]), sizeof(gg) / sizeof(gg[0]));*/


}

int doit() {
	/*wchar_t computerName[MAX_COMPUTERNAME_LENGTH + 1];
	DWORD size = sizeof(computerName) / sizeof(computerName[0]);
	int r = GetComputerNameW(computerName, &size);
	printf("%ls\n", computerName);

	DWORD epic = InternetAttemptConnect(0);
	printf("code was %d\n", epic);

	LPCWSTR url = "https://www.google.com.au";
	printf("url is %s\n", url);
	BOOL yeah = InternetCheckConnectionA(url, FLAG_ICC_FORCE_CONNECTION, 0);
	printf("code of internetcheck was %d\n", yeah);
	printf("last error was %d\n", GetLastError());*/


	HINTERNET hInternetOpen = InternetOpenA(
		"Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:106.0) Gecko/20100101 Firefox/106.0",
		INTERNET_OPEN_TYPE_DIRECT,
		NULL,
		NULL,
		NULL
	);
	printf("handle for internet open %d\n", hInternetOpen);

	LPCSTR addressurl = "www.bom.gov.au";


	HINTERNET hInternetConnect = InternetConnectA(
		hInternetOpen,
		addressurl,
		INTERNET_DEFAULT_HTTP_PORT,
		NULL,
		NULL,
		INTERNET_SERVICE_HTTP,
		0,
		0

	);
	printf("handle for internet connect %d\n", hInternetConnect);

	HINTERNET hRequest = HttpOpenRequestA(
		hInternetConnect,
		"GET",
		"/vic/forecasts/melbourne.shtml",
		"HTTP/1.1",
		NULL,
		NULL,
		NULL,
		0
	);
	printf("handle for http request %d\n", hRequest);

	LPCSTR headers = "Accept: text/html\r\nAccept-Language: en-US,en:q=0.5\r\nAccept-Encoding: identity\r\n Referer: https://www.google.com/\r\nConnection: keep-alive ";

	BOOL dd = HttpAddRequestHeadersA(hRequest, headers, lstrlenA(headers), HTTP_ADDREQ_FLAG_ADD | HTTP_ADDREQ_FLAG_REPLACE);

	printf("error:%d", GetLastError());

	BOOL sendit = HttpSendRequestA(
		hRequest,
		0,
		0,
		0,
		0
	);
	printf("sendit was successful?: %d\n", sendit);
	printf("error: %d\n", GetLastError());

	DWORD available = 0;

	if (InternetQueryDataAvailable(hRequest, &available, 0, 0)) {
		printf("stuff to get! this much: %d\n", available);
	}
	else {
		printf("no stuff to get\n");
	}

	char lpBufferread[32]; //32 spaces 1 bytes wide. 
	int total = 0;
	char* into;
	into = (char*)malloc(8192);
	DWORD bytestoread = 31; //read 31 bytes = 248 bits
	DWORD bytesprocessed;
	char htmllist[1];
	printf("\nstart\n");
	int counter = 0;
	int sizer = 0;
	int over8192counter = 1;
	while (InternetReadFile(hRequest, lpBufferread, bytestoread, &bytesprocessed) == TRUE && bytesprocessed != 0) {
		if (bytesprocessed == 32) {
			lpBufferread[31] = '\0';
		}
		if (bytesprocessed != 32) {
			lpBufferread[bytesprocessed] = '\0';
		}
		total += bytesprocessed;
		printf("\n[*] This call is: %d bytes. Total so far is: %d\n", bytesprocessed, total);
		if (InternetQueryDataAvailable(hRequest, &available, 0, 0)) {
			printf("%d bytes to get...\n\n", available);

		}
		else {
			printf("no stuff to get\n");
			break;
		}
		if (counter == 0) {
			strcpy_s(into, 32, lpBufferread);
			counter += 1;
			printf("first\n\n");
			sizer += 62;
		}
		else if (sizer < over8192counter * 8192) {
			strcat_s(into, sizer + 1, lpBufferread);
			sizer += 31;
			printf("\nsizer:%d\n\n bytes left:%d (distance to next buffer reallocation: %d)\n", sizer, available, (over8192counter * 8192) - sizer);
		}
		else if (sizer > over8192counter * 8192) {
			over8192counter += 1;
			into = (char*)realloc(into, 8192 * over8192counter);
			printf("***********************************************************************************************");
			sizer += 31;
			/*lpBufferread[bytesprocessed] = '\0';*/
			strcat_s(into, sizer, lpBufferread);
		}
		else if (sizer == 8192) {
			printf("----------------REACHED 8192---------------------");
			break;
		}
	}
	printf("\n-------------------------------------------------------------DONE\n");
	//printf("\n%s\n", into);
	printf("\nend \n_____________________processed %d bytes_________________buffer filled up %d times_____________________________\n", total, over8192counter - 1);

	InternetCloseHandle(hRequest);

	// haystacking..

	// snip "into" at the first occurrence of div class day

	/*char* snipped = (char*)malloc(strlen(into));
	snipped = strstr(into, "<div class=\"day\">");*/

	char* string = (char*)malloc(strlen(into));
	string = strstr(into, "div class=\"day main\">");
	char* avoidstring;
	char* avoidstring2;
	
	avoidstring = strstr(string, "div class=\"around\"");
	avoidstring2 = strstr(string, "</table>");

	int avoidA;
	int avoidB;

	avoidA = avoidstring - string;
	avoidB = avoidstring2 - string;
	//printf("----------------------------%d %d", avoidA, avoidB);

	char* holder;
	char* day_of_the_week;

	
	FILE* file;
	errno_t err;
	err = fopen_s(&file, "", "a");

	time_t rawtime;
	struct tm* timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	fprintf(file, "\n\nAppending at %s", asctime(timeinfo));

	int min;
	int max;

	for (int a = 0; a < strlen(string); a++) {
		if (a < avoidA || a > avoidB) {
			if (string[a] == '>') {
				for (int b = a; b <= strlen(string); b++) {
					if (string[b] == '<') {
						holder = (char*)malloc(b - a - 1);
						for (int k = a + 1; k <= b - 1; k++) { // remove +1 and -1 for the >...< being included.
							if (a == 27) {
								holder[abs(a+1 - k)] = string[k]; // You can replace the modulo arithmetic solution with this if you want. Just another way of doing it, that happens to solve my modulo k>2*a bug. 
								printf("_-_-_-_-ABS:%d a:%d holder[k]:%d string[k]:%d string value:%c\n", abs(28-k),a, k % a - 1, k, string[k]);
							}
							else {
								holder[k % a - 1] = string[k];
								printf("_-_-_-_-a:%d holder[k]:%d string[k]:%d string value:%c\n", a, k % a - 1, k, string[k]);
							}

						}
						holder[b - a - 1] = '\0';
						printf("a:%d b:%d\, %s\n", a,b, holder);
						if ((strstr(holder, "Monday") || strstr(holder, "Tuesday") || strstr(holder, "Wednesday") || strstr(holder, "Thursday") || strstr(holder, "Friday") || strstr(holder, "Saturday") || strstr(holder, "Sunday")) && !strstr(holder, "The next routine forecast will be issued at") && !strstr(holder, "UTC")) {
							day_of_the_week = (char*)malloc(b - a);
							strcpy_s(day_of_the_week, b - a + 2, holder);
							printf("----\n%s\n", day_of_the_week);
							fprintf(file, "\n%s,", day_of_the_week);
						}
						if ((strlen(holder) == 2 || strlen(holder) == 1) && isdigit(holder[0])) {
							printf("Found digits: %s\n", holder);
						 	fprintf(file, "%s,", holder);
						}
						break;
					}

				}
			}
		}
		else {
			//printf("\nAVOIDING n : %d", a);
		}


	}
}