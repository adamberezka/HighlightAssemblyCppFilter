#pragma once

#include <Windows.h>
#include<fstream>
#include <string>

void addControls(HWND hWnd);

bool readFile(BYTE* &input, BYTE* &output, LPWSTR fileName, BYTE* &headerPtr);

void saveFile(BYTE* &output, LPWSTR fileName, BYTE* &headerPtr);