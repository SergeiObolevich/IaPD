#include "Battery.h"

#define DELAY 1000

int main() {
	try {
		Battery battery;
		while (!_kbhit()) {
			battery.UpdateStatus();
			cout << battery;
			Sleep(DELAY);
			system("cls");
		}
		system("pause");
		return 0;
	}
	catch (const system_error &exception) {
		cout << endl << "System error with code: " << exception.code() << endl
			<< "Description: " << exception.what() << endl;
		system("pause");
		exit(EXIT_FAILURE);
	}
}