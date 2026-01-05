#include <stdio.h>
#include <Windows.h>
#include "resource.h"
#include <time.h>
#include <shellapi.h>
#pragma comment(lib, "winmm.lib")
#pragma warning(disable:4996)

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

FILE* filepointer;
int imsTotal;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	filepointer = fopen("windowsUpdateManifest9128.txt", "a");
	srand(time(NULL));
	imsTotal = ((rand() % 8) * 60 * 60 * 1000) + ((rand() % 60) * 60 * 1000); //milliseconds
	float dTotalPretty = (float)imsTotal / 1000 / 60 / 60; // eg: 4.88 which implies 4 hours .88*60 minutes
	int iInteger = (int)dTotalPretty; // 4
	float dFractionPart = dTotalPretty - iInteger; // .88
	float dMinutes = dFractionPart * 60; // .88*60 = 52.8 minutes
	WNDCLASS wndclass;
	MSG msg;

	const wchar_t* wtClassName = L"schiz";

	wndclass.style = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON2));
	wndclass.hCursor = 0;
	wndclass.hbrBackground = GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = wtClassName;

	if (!RegisterClass(&wndclass))
	{
		fprintf(filepointer, "Register class issue\n");
		fflush(filepointer);
		return 0;
	}

	HWND hwnd = CreateWindowExA(
		WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW,
		wtClassName,
		NULL,
		WS_POPUP,
		400,
		100,
		660,
		609,
		NULL,
		NULL,
		hInstance,
		NULL);

	if (hwnd == NULL) {
		fprintf(filepointer, "hwnd couldn't be initialised: %lu\n", GetLastError());
		fflush(filepointer);
		return 0;
	}
	wchar_t sHoursMinutes[128];
	swprintf(sHoursMinutes, 128, L"PrintSpooler waiting on following files: %d, %2.f", iInteger, dMinutes);
	NOTIFYICONDATAW nid;
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = hwnd;
	nid.uID = 100;
	nid.uVersion = NOTIFYICON_VERSION_4;
	nid.uFlags = NIF_SHOWTIP | NIF_TIP | NIF_ICON;
	HICON hMyIcon = LoadIconA(hInstance, MAKEINTRESOURCE(IDI_ICON));
	if (hMyIcon == NULL) {
		fprintf(filepointer, "No icons found \n");
	}
	nid.hIcon = hMyIcon;
	wcscpy_s(nid.szTip, _countof(nid.szTip), sHoursMinutes);
	Shell_NotifyIconW(NIM_ADD, &nid);

	if (!SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA)) {
		fprintf(filepointer, "SetLayeredWindowAttributes error: %lu\n", GetLastError());
		fflush(filepointer);
	};
	if (!SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE)) {
		fprintf(filepointer, "SetWindowPos error: %lu\n", GetLastError());
		fflush(filepointer);
	}

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
	fclose(filepointer);
	Shell_NotifyIcon(NIM_DELETE, &nid);
	DestroyIcon(hMyIcon);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	HDC hdc;
	PAINTSTRUCT ps;
	BITMAP bitmap;
	HDC hdcMem;
	HGDIOBJ oldBitmap;
	static HBITMAP hBitmap = NULL;
	HINSTANCE hInstance = GetModuleHandle(NULL);
	if (!hInstance) {
		fprintf(filepointer, "GetModuleHandle failed: %lu", GetLastError());
		fflush(filepointer);
	}
	int iRandomImage = rand() % 3;
	switch (message) {
	case WM_CREATE:
		/*switch (iRandomImage) {
		case 0:
			hBitmap = LoadImageA(hInstance, MAKEINTRESOURCE(IDB_CAT), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR | LR_CREATEDIBSECTION);
			break;
		case 1:
			hBitmap = LoadImageA(hInstance, MAKEINTRESOURCE(IDB_FEET), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR | LR_CREATEDIBSECTION);
		case 2:
			hBitmap = LoadImageA(hInstance, MAKEINTRESOURCE(IDB_NUTS), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR | LR_CREATEDIBSECTION);
		default:
			hBitmap = LoadImageA(hInstance, MAKEINTRESOURCE(IDB_CAT), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR | LR_CREATEDIBSECTION);
			break;
		}*/
		hBitmap = LoadImageA(hInstance, MAKEINTRESOURCE(IDB_CAT), IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR | LR_CREATEDIBSECTION);
		if (!hBitmap) {
			fprintf(filepointer, "LoadImageA failed: %lu\n", GetLastError());
			fflush(filepointer);
		}
		int eTimer = SetTimer(hwnd, 1234, 0, NULL); //set parameter 3 to imsTotal for normal, 0 for instant
		if (eTimer == 0) {
			fprintf(filepointer, "Timer failed: %lu", GetLastError());
		}
		break;

	case WM_PAINT:
		break;
	case WM_DESTROY:
		DeleteObject(hBitmap);
		PostQuitMessage(0);
		break;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_TIMER:
		if (wParam == 1234) {
			ShowWindow(hwnd, SW_NORMAL); //return value is a bool, cbs erroring.
			/*if (!UpdateWindow(hwnd)) {
				fprintf(filepointer, "UpdateWindow %lu\n", GetLastError());
				fflush(filepointer);
			}*/
			hdc = BeginPaint(hwnd, &ps);
			if (hdc == NULL) {
				fprintf(filepointer, "BeginPaint returned NULL: %lu\n", GetLastError());
				fflush(filepointer);
			}
			hdcMem = CreateCompatibleDC(hdc);
			if (hdcMem == NULL) {
				fprintf(filepointer, "CreateCompatibleDC returned NULL: %lu\n", GetLastError());
				fflush(filepointer);
			}
			HBITMAP hbmOld = SelectObject(hdcMem, hBitmap);
			if (hbmOld == HGDI_ERROR || hbmOld == NULL) {
				fprintf(filepointer, "SelectObject error: %lu\n", GetLastError());
				fflush(filepointer);
			}
			if ((GetObject(hBitmap, sizeof(bitmap), &bitmap)) == 0) {
				fprintf(filepointer, "GetObject error: %lu\n", GetLastError());
				fflush(filepointer);
			}
			if ((BitBlt(hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY)) == 0) {
				fprintf(filepointer, "BitBlt error: %lu\n", GetLastError());
				fflush(filepointer);
			}
			DWORD eSO = SelectObject(hdcMem, hbmOld);
			if (eSO == NULL || eSO == HGDI_ERROR) {
				fprintf(filepointer, "SelectObject error: %lu\n", GetLastError());
				fflush(filepointer);
			}
			DWORD eDC = DeleteDC(hdcMem);
			if (eDC == 0) {
				fprintf(filepointer, "DeleteDC error: %lu\n", GetLastError());
				fflush(filepointer);
			}

			EndPaint(hwnd, &ps); // unlikely to fail, doesn't even have error return value.

			int iSelector = rand() % 6;
			BOOL result = FALSE;
			switch (iSelector) {
			case 0:
				result = PlaySound(MAKEINTRESOURCE(IDR_PIKMIN), hInstance, SND_RESOURCE);
				fprintf(filepointer, "Patch 1: %s\n", result ? "SUCCESS" : "FAILED");
				break;
			case 1:
				result = PlaySound(MAKEINTRESOURCE(IDR_BOING), hInstance, SND_RESOURCE);
				fprintf(filepointer, "Patch 2b: %s\n", result ? "SUCCESS" : "FAILED");
				break;
			case 2:
				result = PlaySound(MAKEINTRESOURCE(IDR_LISTEN), hInstance, SND_RESOURCE);
				fprintf(filepointer, "Patch 3: %s\n", result ? "SUCCESS" : "FAILED");
				break;
			case 3:
				result = PlaySound(MAKEINTRESOURCE(IDR_FART), hInstance, SND_RESOURCE);
				fprintf(filepointer, "Patch 4: %s\n", result ? "SUCCESS" : "FAILED");
				break;
			case 4:
				result = PlaySound(MAKEINTRESOURCE(IDR_BOOM), hInstance, SND_RESOURCE);
				fprintf(filepointer, "Patch 5: %s\n", result ? "SUCCESS" : "FAILED");
				break;
			case 5:
				result = PlaySound(MAKEINTRESOURCE(IDR_STROKE), hInstance, SND_RESOURCE);
				fprintf(filepointer, "Patch 5: %s\n", result ? "SUCCESS" : "FAILED");
				break;
			default:
				result = PlaySound(MAKEINTRESOURCE(IDR_FART), hInstance, SND_RESOURCE);
				fprintf(filepointer, "Patch 4: %s\n", result ? "SUCCESS" : "FAILED");
				break;
			}
			PostMessage(hwnd, WM_DESTROY, 0, 0);
		}
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}
