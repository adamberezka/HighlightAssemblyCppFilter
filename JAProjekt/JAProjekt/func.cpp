#include "func.h"


void addControls(HWND hWnd) {

	CreateWindowW(L"static", L"Wybierz plik:", WS_VISIBLE | WS_CHILD | SS_CENTER, 20, 20, 100, 20, hWnd, nullptr, nullptr, nullptr);
	CreateWindowW(L"Edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL, 140, 20, 360, 20, hWnd, (HMENU)300, nullptr, nullptr);
	CreateWindowW(L"Button", L". . .", WS_VISIBLE | WS_CHILD | SS_CENTER, 520, 20, 90, 20, hWnd, (HMENU)301, nullptr, nullptr);

	CreateWindowW(L"static", L"Wywo³aj funkcjê napisan¹ w:", WS_VISIBLE | WS_CHILD | SS_CENTER , 120, 60, 200, 20, hWnd, nullptr, nullptr, nullptr);
	CreateWindowW(L"Button", L"C++", WS_VISIBLE | WS_CHILD | SS_CENTER| BS_AUTORADIOBUTTON , 340, 60, 60, 20, hWnd, (HMENU)302, nullptr, nullptr);
	CreateWindowW(L"Button", L"ASM", WS_VISIBLE | WS_CHILD | ES_CENTER| BS_AUTORADIOBUTTON , 420, 60, 60, 20, hWnd, (HMENU)303, nullptr, nullptr);
	CheckRadioButton(hWnd, 302, 303, 302);

	CreateWindowW(L"static", L"Wybierz iloœæ w¹tków:", WS_VISIBLE | WS_CHILD | SS_CENTER , 225, 100, 150, 20, hWnd, nullptr, nullptr, nullptr);
	CreateWindowW(L"Edit", L"1", WS_VISIBLE | WS_CHILD | WS_BORDER, 395, 100, 20, 20, hWnd, (HMENU)304, nullptr, nullptr);

	CreateWindowW(L"Button", L"Wykonaj", WS_VISIBLE | WS_CHILD | ES_CENTER, 220, 140, 200, 20, hWnd, (HMENU)305, nullptr, nullptr);

	CreateWindowW(L"static", L"Czas wykonania: ", WS_VISIBLE | WS_CHILD , 240, 180, 120, 20, hWnd, nullptr, nullptr, nullptr);
	CreateWindowW(L"static", L"--", WS_VISIBLE | WS_CHILD, 360, 180, 200, 20, hWnd, (HMENU)307, nullptr, nullptr);
	CreateWindowW(L"Button", L"Zapisz", WS_VISIBLE | WS_CHILD | ES_CENTER, 220, 220, 200, 20, hWnd, (HMENU)308, nullptr, nullptr);
}

bool readFile(BYTE*& input, BYTE*& output, LPWSTR fileName, BYTE*& headerPtr) {

	/*std::string fn(fileName);
	if (fn.substr(fn.find_last_of(".") + 1) != "bmp") {
		return false;
	}*/

	const size_t HEADER_SIZE = 54;

	int padding = 0;

	std::ifstream bmp(fileName, std::ios::binary);

	if (bmp.is_open() != true) {
		return false;
	}
	if (headerPtr == nullptr) {
		headerPtr = new BYTE[HEADER_SIZE];
	}
	bmp.read((char*)headerPtr, HEADER_SIZE);

	auto width = *reinterpret_cast<uint32_t*>(&headerPtr[18]);
	auto height = *reinterpret_cast<uint32_t*>(&headerPtr[22]);

	if (output == nullptr) {
		output = new BYTE[(((width * 3) + padding) * height)];
		input = new BYTE[(((width * 3) + padding) * height)];
	}

	if ((width * 3) % 4 != 0)
		padding = 4 - ((width * 3) % 4);

	

	bmp.read((char*)input, (((width * 3) + padding) * height));
	bmp.close();

	for (int i = 0; i < (width * 3) + padding; i++) {
		for (int j = 0; j < height; j++) {
			output[j * ((width * 3) + padding) + i] = 0; /////////////////////////
		}
	}

	return true;
}

void saveFile(BYTE*& output, LPWSTR fileName, BYTE*& headerPtr) {

	const size_t HEADER_SIZE = 54;

	int padding = 0;

	auto width = *reinterpret_cast<uint32_t*>(&headerPtr[18]);
	auto height = *reinterpret_cast<uint32_t*>(&headerPtr[22]);

	if ((width * 3) % 4 != 0)
		padding = 4 - ((width * 3) % 4);

	std::ofstream outputFile(fileName, std::ios::binary);
	outputFile.write((char*)headerPtr, HEADER_SIZE);
	outputFile.write((char*)output, (((width * 3) + padding) * height));
	outputFile.close();

}