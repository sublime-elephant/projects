// hoodirony.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "hoodirony.h"
#include "Dshow.h"
#include <uuids.h>

#pragma comment(lib, "mfplay.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "Strmiids.lib")
#pragma comment(lib, "Quartz.lib")

#define MAX_LOADSTRING 100
#define WM_GRAPHNOTIFY WM_APP + 1


// Global Variables:
HRESULT hr;
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
IGraphBuilder* pGraph;
IBaseFilter* pVMR7;
IVMRFilterConfig* pVMRfilterconfig7;
IVMRWindowlessControl* pVMRcontrol7;
IMediaControl* pControl;
IMediaEventEx* pEvent;
long lWidth, lHeight;
WCHAR buf[64];

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
void PlayVideo(HWND, const WCHAR*);
int ErrorCheck(HRESULT, char**, HWND);



int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    


    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_HOODIRONY, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }


    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, NULL, 0, 0))
    {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
    }

    return (int) msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = 0;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon = NULL;
    wcex.hCursor        = NULL;
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm = NULL;

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE, szWindowClass, NULL, WS_POPUP, 400, 100, 400, 400, NULL, NULL, hInstance, NULL);

   if (!hWnd) {
       WCHAR buf3[250];
       DWORD err;
       err = GetLastError();
       swprintf_s(buf3, 250, L"failedHwnd: %lu", err);
       MessageBoxW(NULL, buf3, L"Error", MB_OK);
       return FALSE;
   }
   SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);

   ShowWindow(hWnd, SW_SHOWNOACTIVATE);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{


    // virtual class IDrawable ... virtual void draw() = 0 

    // class Shape : public IDrawable
    // {
    //    draw(your own way)
    //}

    // class Bitmap : public IDrawable
    // { draw(your own way) }

    //IDrawable* pShape;
    //CoCreateInstance(CLSID_Shape, ..., ..., &IID_IDrawable, ..., (void**)&pShape); i.e., Create the Shape object and give me a
    // pointer to the IDrawable interface, which contains the  definition for a "barebones" draw() method.

    //Interfaces of an object are like functionality toolboxes. You can extend as many interfaces on an object as you need, to achieve
    //desired functionality.

    //The "Open file" dialog box can just expose the IFileOpenDialog for basic use. But, if you additionally expose IFileDialogCustomize
    //you gain access to more functions.

    //QueryInterface is used to hop between interfaces of the same object.



    switch (message)
    {
    case WM_CREATE:
        hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        hr = CoCreateInstance(&CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, &IID_IGraphBuilder, (void**)&pGraph);
        ErrorCheck(hr, buf, hWnd);

        hr = CoCreateInstance(&CLSID_VideoMixingRenderer, NULL, CLSCTX_INPROC_SERVER, &IID_IBaseFilter, (void**)&pVMR7);
        ErrorCheck(hr, buf, hWnd);

        hr = pGraph->lpVtbl->AddFilter(pGraph, pVMR7, NULL);
        ErrorCheck(hr, buf, hWnd);
        hr = pVMR7->lpVtbl->QueryInterface(pVMR7, &IID_IVMRFilterConfig, &pVMRfilterconfig7);
        ErrorCheck(hr, buf, hWnd);
        hr = pVMRfilterconfig7->lpVtbl->SetRenderingMode(pVMRfilterconfig7, VMRMode_Windowless);
        ErrorCheck(hr, buf, hWnd);
        hr = pVMR7->lpVtbl->QueryInterface(pVMR7, &IID_IVMRWindowlessControl, &pVMRcontrol7);
        ErrorCheck(hr, buf, hWnd);
        hr = pVMRcontrol7->lpVtbl->GetNativeVideoSize(pVMRcontrol7, &lWidth, &lHeight, NULL, NULL);
        ErrorCheck(hr, buf, hWnd);
        RECT rcSRC, rcDEST;
        GetClientRect(hWnd, &rcDEST);
        SetRect(&rcDEST, 0, 0, rcDEST.right, rcDEST.bottom);
        hr = pVMRcontrol7->lpVtbl->SetVideoPosition(pVMRcontrol7, NULL, &rcDEST);
        ErrorCheck(hr, buf, hWnd);
        hr = pVMRcontrol7->lpVtbl->SetVideoClippingWindow(pVMRcontrol7, hWnd);
        ErrorCheck(hr, buf, hWnd);

        //temp-file hack

        HGLOBAL hResource;
        HGLOBAL hLoadResource;
        static WCHAR lpNewFile[MAX_PATH];
        WCHAR buf2[250];
        HANDLE hFile;
        srand(time(NULL));
        int iRand = (rand() % 3);
        switch (iRand) {
        case 0:
            hResource = FindResource(NULL, MAKEINTRESOURCE(IDR_ASF1), RT_RCDATA);
            break;
        case 1:
            hResource = FindResource(NULL, MAKEINTRESOURCE(IDR_BABY), RT_RCDATA);
            break;
        case 2:
            hResource = FindResource(NULL, MAKEINTRESOURCE(IDR_FAWK), RT_RCDATA);
            break;
        default:
            hResource = FindResource(NULL, MAKEINTRESOURCE(IDR_ASF1), RT_RCDATA);
            break;
        }
        if (hResource == NULL) {
            DWORD err;
            err = GetLastError();
            swprintf_s(buf2, 250, L"Error1: %lu", err);
            MessageBoxW(hWnd, buf2, NULL, MB_OK);
            PostMessage(hWnd, WM_DESTROY, 0, 0);
        }
        else {
            DWORD sizeofResource = SizeofResource(NULL, hResource);
            if (sizeofResource == 0) {
                DWORD err;
                err = GetLastError();
                swprintf_s(buf2, 250, L"Error2: %lu", err);
                MessageBoxW(hWnd, buf2, NULL, MB_OK);
                PostMessage(hWnd, WM_DESTROY, 0, 0);
            }
            else {
                hLoadResource = LoadResource(NULL, hResource);
                if (hResource == NULL) {
                    DWORD err;
                    err = GetLastError();
                    swprintf_s(buf2, 250, L"Error3: %lu", err);
                    MessageBoxW(hWnd, buf2, NULL, MB_OK);
                    PostMessage(hWnd, WM_DESTROY, 0, 0);
                }
                else {
                    LPVOID lpResource;
                    lpResource = LockResource(hLoadResource);
                    if (lpResource == NULL) {
                        DWORD err;
                        err = GetLastError();
                        swprintf_s(buf2, 250, L"Obtaining pointer to resource failed: %lu", err);
                        MessageBoxW(hWnd, buf2, NULL, MB_OK);
                        PostMessage(hWnd, WM_DESTROY, 0, 0);
                    }
                    else {
                        UINT iFilename;
                        DWORD dwTempDir;
                        TCHAR pathBuffer[MAX_PATH + 1];

                        TCHAR* errorCheck[60];
                        dwTempDir = GetTempPathW(MAX_PATH + 1, &pathBuffer);
                        swprintf_s(errorCheck, 60, L"bytes: %lu", dwTempDir);
                        if (dwTempDir == 0) {
                            DWORD err;
                            err = GetLastError();
                            swprintf_s(buf2, 250, L"4failed: %lu", err);
                            MessageBoxW(hWnd, buf2, NULL, MB_OK);
                            PostMessage(hWnd, WM_DESTROY, 0, 0);
                        }
                        else {
                            iFilename = GetTempFileNameW(pathBuffer, L"bje", 0, lpNewFile);
                            if (iFilename == 0) {
                                DWORD err;
                                err = GetLastError();
                                swprintf_s(buf2, 250, L"failedTempFileName: %lu", err);
                                MessageBoxW(hWnd, buf2, NULL, MB_OK);
                                PostMessage(hWnd, WM_DESTROY, 0, 0);
                            }
                            else {
                                hFile = CreateFileW(lpNewFile, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, NULL);
                                if (hFile == INVALID_HANDLE_VALUE) {
                                    DWORD err;
                                    err = GetLastError();
                                    swprintf_s(buf2, 250, L"5failed: %lu", err);
                                    MessageBoxW(hWnd, buf2, NULL, MB_OK);
                                    PostMessage(hWnd, WM_DESTROY, 0, 0);
                                }
                                else {
                                    DWORD nBytesWritten;
                                    if (WriteFile(hFile, lpResource, sizeofResource, &nBytesWritten, NULL) == 0) {
                                        DWORD err;
                                        err = GetLastError();
                                        swprintf_s(buf2, 250, L"filewrite: %lu", err);
                                        MessageBoxW(hWnd, buf2, NULL, MB_OK);
                                        PostMessage(hWnd, WM_DESTROY, 0, 0);
                                    }
                                    else {
                                        CloseHandle(hFile);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        //MessageBoxW(hWnd, &lpNewFile, NULL, MB_OK);
        hr = pGraph->lpVtbl->RenderFile(pGraph, &lpNewFile, NULL);
        ErrorCheck(hr, buf, hWnd);
        hr = pGraph->lpVtbl->QueryInterface(pGraph, &IID_IMediaControl, &pControl);
        ErrorCheck(hr, buf, hWnd);
        hr = pGraph->lpVtbl->QueryInterface(pGraph, &IID_IMediaEventEx, (void**)&pEvent);
        ErrorCheck(hr, buf, hWnd);
        hr = pEvent->lpVtbl->SetNotifyWindow(pEvent, hWnd, WM_GRAPHNOTIFY, 0);
        hr = pControl->lpVtbl->Run(pControl);
        ErrorCheck(hr, buf, hWnd);


        break;

    case WM_APP+1:
    {
        long evCode;
        LONG_PTR param1;
        LONG_PTR param2;
        while (SUCCEEDED(pEvent->lpVtbl->GetEvent(pEvent, &evCode, &param1, &param2, 0))) {
            pEvent->lpVtbl->FreeEventParams(pEvent, evCode, param1, param2);
            switch (evCode) {
            case EC_COMPLETE:
            case EC_USERABORT:
            case EC_ERRORABORT:
                PostMessage(hWnd, WM_DESTROY, 0, 0);
                break;
            }
        }
        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: Add any drawing code that uses hdc here...
        hr = pVMRcontrol7->lpVtbl->RepaintVideo(pVMRcontrol7, hWnd, hdc);
        ErrorCheck(hr, buf, hWnd);
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_DESTROY:
        pGraph->lpVtbl->Release(pGraph);
        pVMR7->lpVtbl->Release(pVMR7);
        pControl->lpVtbl->Release(pControl);
        pVMRcontrol7->lpVtbl->Release(pVMRcontrol7);
        pVMRfilterconfig7->lpVtbl->Release(pVMRfilterconfig7);
        CoUninitialize();
        DeleteFile(&lpNewFile);
        PostQuitMessage(0);

        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    return 0;
    }
    
}

int ErrorCheck(HRESULT hr, WCHAR** buf, HWND hWnd) {
    if (FAILED(hr)) {
        swprintf_s(buf, 64, L"Error was: %lu", hr);
        MessageBoxW(hWnd, buf, NULL, MB_OK);
        return 1;
    }
}