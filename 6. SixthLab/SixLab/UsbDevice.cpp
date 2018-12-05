#include <exception>
#include <iostream>
#include "UsbDevice.h"

UsbDevice::UsbDevice(string letter) {
	this->letter = letter;

	this->link = "\\\\.\\";
	this->link.push_back(letter.at(0));
	this->link.push_back(':');

	this->getDeviceInfo();
	this->getDeviceSize();

	hDrive = CreateFile(link.c_str(), GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

	if (hDrive == INVALID_HANDLE_VALUE) {
		throw exception("Can't get access to drive.");
	}
}

UsbDevice::~UsbDevice() {
	if (hDrive != INVALID_HANDLE_VALUE && hDrive != NULL) {
		CloseHandle(hDrive);
	}
}

void UsbDevice::getDeviceInfo() {
	char drive_name[MAX_PATH] = { 0 };
	char fs[MAX_PATH] = { 0 };

	BOOL succed_flag = GetVolumeInformation(letter.c_str(), drive_name, MAX_PATH,
		NULL, NULL, NULL, fs, MAX_PATH);

	if (succed_flag == TRUE) {
		this->file_sys = fs;
		this->name = drive_name;
	}
	else {
		throw exception("Can't get drive information");
	}
}

void UsbDevice::getDeviceSize() {
	ULARGE_INTEGER full;
	ULARGE_INTEGER free;

	GetDiskFreeSpaceEx(letter.c_str(), NULL, &full, &free);

	this->full_space = full.QuadPart;
	this->free_space = free.QuadPart;
	this->busy_space = full.QuadPart - free.QuadPart;
}

int UsbDevice::lockDevice() {
	DWORD dwBytesReturned;
	DWORD dwSleepTime;
	int nTrys;

	const int TIMEOUT = 10000;
	const int TRYS = 20;

	dwSleepTime = TIMEOUT / TRYS;

	for (nTrys = 0; nTrys < TRYS; nTrys++) {
		if (!DeviceIoControl(hDrive, FSCTL_LOCK_VOLUME,
			NULL, 0, NULL, 0, &dwBytesReturned, NULL)) {
			return 0;
		}
		else {
			return 1;
		}
	}
}

void UsbDevice::dismountDevice() {
	DWORD dwBytesReturned;

	BOOL bResult = DeviceIoControl(hDrive, FSCTL_DISMOUNT_VOLUME,
		NULL, 0, NULL, 0, &dwBytesReturned, NULL);

	if (bResult == FALSE) {
		throw exception("Can't dismount volume");
	}
}

void UsbDevice::removeMedia() {
	DWORD dwBytesReturned;
	PREVENT_MEDIA_REMOVAL mediaRemoval;

	mediaRemoval.PreventMediaRemoval = FALSE;

	BOOL bResult = DeviceIoControl(hDrive, IOCTL_STORAGE_MEDIA_REMOVAL,
		&mediaRemoval, sizeof(PREVENT_MEDIA_REMOVAL),
		NULL, 0, &dwBytesReturned, NULL);

	if (bResult == FALSE) {
		throw exception("Can't remove media");
	}
}

void UsbDevice::ejectMedia() {
	DWORD dwBytesReturned;

	BOOL bResult = DeviceIoControl(hDrive, IOCTL_STORAGE_EJECT_MEDIA,
		NULL, 0, NULL, 0, &dwBytesReturned, NULL);

	if (bResult == FALSE) {
		throw exception("Can't eject media");
	}
}

int UsbDevice::lock() {
	int check = lockDevice();
	return check;
}

int UsbDevice::safetyRemove() {
	int check = lockDevice();
	if (check == 1) {
		dismountDevice();
		removeMedia();
		ejectMedia();
		CloseHandle(hDrive);
		return 1;
	}
	else {
		cout << "Can't remove device, please close all files." << endl;
		system("pause");
		return 0;
	}
}

int UsbDevice::badlyRemove() {
	string data;
	cout << "If you want do unsafety remove device, please tab 'yes', else tan 'no': ";
	getline(cin, data);
	if (data == "yes") {
		dismountDevice();
		removeMedia();
		ejectMedia();
		CloseHandle(hDrive);
		return 1;
	}
	else if (data == "no") {
		return 0;
	}
}

string UsbDevice::getName() {
	return this->name;
}

string UsbDevice::getLetter() {
	return this->letter;
}

void UsbDevice::showInfo() {
	cout << "Drive Letter  : " << letter << endl;
	cout << "Drive Name    : " << name << endl;
	cout << "File system   : " << file_sys << endl;
	cout << "Memory (t/b/f): " << full_space / megabytes
		<< "/" << busy_space / megabytes
		<< "/" << free_space / megabytes << " MB" << endl;
}