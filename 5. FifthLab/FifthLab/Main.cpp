#include <iostream>
#include <fstream>
#include <Windows.h>
#include <tlhelp32.h>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

enum ReplacementMode { 
	HIDE = 1, 
	EMULATE = 2 
};

struct replacement {
	string key;
	ReplacementMode action;
	string description;
};

vector<replacement> replacementVector;

PKBDLLHOOKSTRUCT hookStruct;
HHOOK keyboardHook, mouseHook;
HWND windowHandler;
MSG message;
char keyboardFileName[] = "keyboardFile.txt";
char mouseFileName[] = "mouseFile.txt";
int windowStatus;
string temp, emulatedKey, hideKey;
bool changeMouseKeys = false;

void interceptAction(string key, char* fileName) {
	ofstream log(fileName, ios::app);
	log << key << endl;
	log.close();
}

DWORD sendVirtualKey(string key, BOOL key_up) {
	int scanÑode = OemKeyScan(key[0]) & 0x0ff;
	if (scanÑode) {
		INPUT inp;
		ZeroMemory(&inp, sizeof(inp));
		inp.type = INPUT_KEYBOARD;
		inp.ki.wScan = scanÑode;
		inp.ki.dwFlags = KEYEVENTF_SCANCODE | (key_up ? KEYEVENTF_KEYUP : 0);
		return SendInput(1, &inp, sizeof(inp)) ? NO_ERROR : GetLastError();
	}
	else {
		return ERROR_NO_UNICODE_TRANSLATION;
	}
}

void emulateKey(string key) {
	sendVirtualKey(key, FALSE);
	Sleep(100);
	sendVirtualKey(key, TRUE);
}

string vkToText(DWORD vk_code) {
	char buffer[16];
	int scanÑode = MapVirtualKey(vk_code, 0);
	scanÑode <<= 16;

	GetKeyNameTextA(scanÑode, buffer, sizeof(buffer));
	string key = buffer;

	if (!((GetKeyState(VK_CAPITAL) & 0x0001) != 0 || ((GetKeyState(VK_SHIFT) & 0x8000) != 0))) {
		transform(key.begin(), key.end(), key.begin(), ::tolower);
	}
	return key;
}

void addAction(int currentReplacedNumber, ReplacementMode action, string actionArgument) {
	if (replacementVector.size() == currentReplacedNumber) {
		replacement replacementKeyStructure;
		replacementKeyStructure.key = temp;
		replacementKeyStructure.action = action;
		replacementKeyStructure.description = actionArgument;
		replacementVector.push_back(replacementKeyStructure);
	}
	else {
		replacementVector.at(currentReplacedNumber).action = action;
		replacementVector.at(currentReplacedNumber).description = actionArgument;
	}
}

void waitMessage() {
	while (GetMessage(&message, NULL, 0, 0)) {
		TranslateMessage(&message);
		DispatchMessage(&message);
	}
}

void newReplacement() {
	int currentReplacedNumber = replacementVector.size();

	system("cls");
	cout << "Press key to add new replacement: " << endl;
	rewind(stdin);
	fflush(stdin);
	cin >> temp;

	while (true) {
		fflush(stdin);
		cout << "1. Hide input key.";
		cout << endl << "2. Replace input key to another.";
		cout << endl << "0. Finish.";
		cout << endl << "Your choice: ";

		int i;	
		do {
			rewind(stdin);
			fflush(stdin);
		} while (!scanf_s("%d", &i));

		switch (i) {
			case 1: {
				addAction(currentReplacedNumber, ReplacementMode::HIDE, "hide status");
				break;
			}	
			case 2: {
				cout << "Input key to replace: ";
				rewind(stdin);
				cin >> emulatedKey;
				addAction(currentReplacedNumber, ReplacementMode::EMULATE, emulatedKey);
				break;
			}
			case 0: {
				break;
			}
			default: {
				break;
			}
		}
		if (i == 0 || i == 1 || i == 2) {
			break;
		}
	}
}

DWORD WINAPI hookThread(CONST LPVOID l_param) {
	while (true) {
		system("cls");
		cout << "1. Add new replacement.";
		cout << endl << "2. Set window hide button.";
		cout << endl << "3. Swap mouse buttons.";
		cout << endl << "Your choice: ";

		int i;
		do {
			rewind(stdin);
			fflush(stdin);
		} while (!scanf_s("%d", &i));

		switch (i) {
			case 1: {
				newReplacement();
				break;
			}			
			case 2: {
				cout << endl << "Press key to select hide button: ";
				cin >> hideKey;
				break;
			}
			case 3: {
				changeMouseKeys = !changeMouseKeys;
			}
			default: {
				cout << endl << "Unknown Command.";
				break;
			}
		}
	}
}

LRESULT CALLBACK keyboardHookProc(int n_code, WPARAM w_param, LPARAM l_param) {
	int code = 0;

	if (w_param == WM_KEYDOWN) {
		PKBDLLHOOKSTRUCT keyboardHook_struct = (PKBDLLHOOKSTRUCT)(l_param);
		PROCESS_INFORMATION process_information;
		string key = vkToText(keyboardHook_struct->vkCode);

		if (key == hideKey) {
			ShowWindow(windowHandler, windowStatus);
			code = 1;
			if (windowStatus == 0)
				windowStatus = 5;
			else
				windowStatus = 0;
		}

		for (vector<replacement>::iterator it = replacementVector.begin(); it != replacementVector.end(); it++) {
			if (it->key != key) {
				continue;
			}
			switch (it->action) {
				case ReplacementMode::HIDE: {
					code = 0;
					break;
				}
				case ReplacementMode::EMULATE: {
					emulateKey(it->description);
					code = 1;
					break;
				}
				default:
					break;
				}
			code = 1;
		}
		interceptAction(key, keyboardFileName);
	}
	return code;
}

LRESULT CALLBACK mouseHookProc(int n_code, WPARAM w_param, LPARAM l_param) {
	if (w_param == WM_RBUTTONDOWN) {
		interceptAction("RightClick", mouseFileName);
	}
	if (w_param == WM_LBUTTONDOWN) {
		interceptAction("LeftClick", mouseFileName);
	}
	SwapMouseButton(changeMouseKeys);
	return 0;
}

int main() {
	windowStatus = 0;

	windowHandler = FindWindowA("ConsoleWindowClass", nullptr);

	mouseHook = SetWindowsHookEx(WH_MOUSE_LL, mouseHookProc, nullptr, 0);
	keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboardHookProc, nullptr, 0);

	HANDLE thread = CreateThread(nullptr, 0, &hookThread, nullptr, 0, nullptr);

	waitMessage();
	return 0;
}