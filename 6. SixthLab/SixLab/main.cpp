#include <Windows.h>
#include <iostream>
#include "DeviceScanner.h"

void refresh(DeviceScanner* ref);

int main() {
	DeviceScanner *scan;
	scan = new DeviceScanner();

	auto count = 0;

	try {
		while (true) {
			if (count == 0) {
				scan->listUsbDevices();
				scan->listMtpDevices();
				scan->printDrives();

				count++;
			}

			string data;

			cout << endl << "Please enter command: ";
			getline(cin, data);

			if (data.substr(0, 6) == "delete") {
				bool exist = scan->ejectDevice(data.substr(7, 9));
				if (exist) {
					cout << "Device \'" << data.substr(7, 9)
						<< "\' ejected." << endl;
					cout << "Please, remove drive from pc." << endl;
					getchar();
				}
				else {
					cout << "Device \'" << data.substr(7, 9)
						<< "\' not exist." << endl;
				}
				refresh(scan);

			}
			else if (data.substr(0, 6) == "sdelet") {
				bool exist = scan->ejectDeviceS(data.substr(7, 9));
				if (exist) {
					cout << "Device \'" << data.substr(7, 9)
						<< "\' ejected." << endl;
					cout << "Please, remove drive from pc." << endl;
					getchar();
				}
				else {
					cout << "Device \'" << data.substr(7, 9)
						<< "\' not exist." << endl;
				}
				refresh(scan);

			}
			else if (data == "quit") {
				delete scan;
				system("pause");
				return 0;
			}
			else {
				refresh(scan);
			}
		}
	}
	catch (exception e) {
		cout << e.what();
	}

	system("pause");
	return 0;
}

void refresh(DeviceScanner* ref) {
	system("cls");
	ref->listUsbDevices();
	ref->listMtpDevices();
	ref->printDrives();
}