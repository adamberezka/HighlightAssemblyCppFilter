// JAProjekt.cpp : Definiuje punkt wejścia dla aplikacji.
//
#define _CRT_SECURE_NO_WARNINGS

#include <chrono>
#include "framework.h"
#include "JAProjekt.h"
#include "CppDll.h"
#include "func.h";
#include <fstream>
#include <vector>
#include <thread>
#include <Commdlg.h>

#define MAX_LOADSTRING 100
#define _WINNT_WIN32 0600

// Zmienne globalne:
HINSTANCE hInst;                                // bieżące wystąpienie
WCHAR szTitle[MAX_LOADSTRING];                  // Tekst paska tytułu
WCHAR szWindowClass[MAX_LOADSTRING];            // nazwa klasy okna głównego

// Przekaż dalej deklaracje funkcji dołączone w tym module kodu:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);



int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine); 

    // TODO: W tym miejscu umieść kod.

    // Inicjuj ciągi globalne
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_JAPROJEKT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Wykonaj inicjowanie aplikacji:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_JAPROJEKT));

    MSG msg;

    // Główna pętla komunikatów:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNKCJA: MyRegisterClass()
//
//  PRZEZNACZENIE: Rejestruje klasę okna.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_JAPROJEKT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName   = nullptr;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNKCJA: InitInstance(HINSTANCE, int)
//
//   PRZEZNACZENIE: Zapisuje dojście wystąpienia i tworzy okno główne
//
//   KOMENTARZE:
//
//        W tej funkcji dojście wystąpienia jest zapisywane w zmiennej globalnej i
//        jest tworzone i wyświetlane okno główne programu.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Przechowuj dojście wystąpienia w naszej zmiennej globalnej

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      500, 300, 640, 300, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNKCJA: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PRZEZNACZENIE: Przetwarza komunikaty dla okna głównego.
//
//  WM_COMMAND  - przetwarzaj menu aplikacji
//  WM_PAINT    - Maluj okno główne
//  WM_DESTROY  - opublikuj komunikat o wyjściu i wróć
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static bool file = false;
	static BYTE* pixels = nullptr;
	static BYTE* output = nullptr;
	static BYTE* header = nullptr;
	static int threads = 1;
	static int padding = 0;
	static int baseRows = 0;
	static int additionalRows = 0;
	static TCHAR szFileName[260] = { 0 };
	static LPWSTR fileName = szFileName;
	static TCHAR oldszFileName[260] = { 0 };
	static LPWSTR oldfileName = oldszFileName;
	static OPENFILENAME ofn = { 0 };
	static TCHAR szFile[260] = { 0 };
	static TCHAR FilePath[260] = { 0 };
	static std::vector<std::thread> threadVec;
	static OPENFILENAME SaveFileName;
	typedef void(_fastcall* filterASMType)(BYTE* data, BYTE* output, int width, int padding, int start, int count);
	static HINSTANCE dllHandle = NULL;
	if (dllHandle == NULL)
		dllHandle = LoadLibrary(L"AsmDll.dll");
	static filterASMType filterASM = (filterASMType)GetProcAddress(dllHandle, "filterASM");

    switch (message)
    {
	case WM_CREATE:
		{
		addControls(hWnd);
		}
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Analizuj zaznaczenia menu:
			switch (wmId)
			{
			case 301: // Przycisk wyboru pliku
				ofn.lStructSize = sizeof(ofn);
				ofn.hwndOwner = hWnd;
				ofn.lpstrFile = szFile;
				ofn.nMaxFile = sizeof(szFile);
				ofn.lpstrFilter = _T("*.bmp\0*.bmp\0\0");
				ofn.nFilterIndex = 1;
				ofn.lpstrFileTitle = NULL;
				ofn.nMaxFileTitle = 0;
				ofn.lpstrInitialDir = NULL;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

				if (GetOpenFileName(&ofn) == TRUE)
					SetDlgItemText(hWnd, 300, ofn.lpstrFile);

				delete[] output;
				delete[] pixels;
				delete[] header;
				output = nullptr;
				pixels = nullptr;
				header = nullptr;

				file = readFile(pixels, output, ofn.lpstrFile, header);
				

				break;
			case 305: // Przycisk wykonania filtrowania

				threads = 1;
				wchar_t buffer[256];
				wsprintfW(buffer, L"--");
				SetDlgItemText(hWnd, 307, buffer);
				if(file == false){
					MessageBox(hWnd, L"Nie można otowrzyć pliku!", L"Błąd", MB_OK);
				}else if (threads < 1 || threads > 64) {
					MessageBox(hWnd, L"Wprowadź prawidłową ilość wątków (od 1 do 64)!", L"Błąd", MB_OK);
				} else {
					threads = GetDlgItemInt(hWnd, 304, nullptr, false);
					auto width = *reinterpret_cast<uint32_t*>(&header[18]);
					auto height = *reinterpret_cast<uint32_t*>(&header[22]);
					if ((width * 3) % 4 != 0)
						padding = 4 - ((width * 3) % 4);
					baseRows = (height - 2) / threads;
					additionalRows = (height - 2) % threads;
					auto start = std::chrono::high_resolution_clock::now();
					if (IsDlgButtonChecked(hWnd, 302)) {
						for (int i = 0; i < additionalRows; i++)
							threadVec.push_back(std::thread(filterCPP, pixels, output, width, padding, i*(baseRows + 1) + 1 ,baseRows + 1)); 
						for (int i = 0; i < threads - additionalRows; i++)
							threadVec.push_back(std::thread(filterCPP, pixels, output, width, padding, (((baseRows + 1) * additionalRows) + 1) + (i * baseRows), baseRows));
						for (std::thread& th : threadVec)
							if (th.joinable())
								th.join();
					} else {
						for (int i = 0; i < additionalRows; i++)
							threadVec.push_back(std::thread(filterASM, pixels, output, width, padding, i * (baseRows + 1) + 1, baseRows + 1));
						for (int i = 0; i < threads - additionalRows; i++)
							threadVec.push_back(std::thread(filterASM, pixels, output, width, padding, (((baseRows + 1) * additionalRows) + 1) + (i * baseRows), baseRows));
						for (std::thread& th : threadVec)
							if (th.joinable())
								th.join();
					}
					auto finish = std::chrono::high_resolution_clock::now();
					auto duration = std::chrono::duration_cast<std::chrono::microseconds>(finish - start);
					int time = duration.count();
					wsprintfW(buffer, L"%d %c%c", time, 0x0B5, 's');
					SetDlgItemText(hWnd, 307, buffer);
					threadVec.empty();
				}
				break;
			case 308: //Przycisk zapisz
				SaveFileName.lStructSize = sizeof(OPENFILENAME);
				SaveFileName.hwndOwner = hWnd;
				SaveFileName.lpstrFile = FilePath;
				SaveFileName.nMaxFile = sizeof(FilePath);
				SaveFileName.lpstrFilter = _T("*.bmp\0*.bmp\0\0");
				GetSaveFileName(&SaveFileName);
				saveFile(output, SaveFileName.lpstrFile, header);
				break;
            case IDM_EXIT:
				delete[] output;
				delete[] pixels;
				delete[] header;
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}