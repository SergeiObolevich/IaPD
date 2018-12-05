#include <Windows.h>
#include <objbase.h>

#include <codecvt>
#include <locale>
#include <iostream>
#include <sstream>
#include <tuple>
#include <vector>

#include "DeviceScanner.h"

DeviceScanner::DeviceScanner() {
	this->prev_usb = 100;
	this->prev_mtp = 100;
}

DeviceScanner::~DeviceScanner() {
	clearUsbList();
	clearMtpList();
}

void DeviceScanner::clearUsbList() {
	if (usb_dev.size() != 0) {
		for (auto entry : usb_dev) {
			delete entry;
		}
		usb_dev.clear();
	}
}

void DeviceScanner::clearMtpList() {
	if (mtp_dev.size() != 0) {
		for (auto entry : mtp_dev) {
			delete entry;
		}
		mtp_dev.clear();
	}
}

tuple<string, string> DeviceScanner::identifyMtpDevice(_In_ IPortableDeviceManager* devMgr, _In_ PCWSTR id) {
	DWORD nameLength = 0;
	DWORD manNameLength = 0;

	wstringstream name;
	wstringstream manufacturer;

	HRESULT hName;
	HRESULT hMan;

	hName = devMgr->GetDeviceFriendlyName(id, nullptr, &nameLength);
	hMan = devMgr->GetDeviceManufacturer(id, nullptr, &manNameLength);

	if (FAILED(hName) || FAILED(hMan)) {
		throw exception("Failed to get number of characters"
			"for device friendly or manufacturer name");
	}
	else if (nameLength > 0 && manNameLength > 0) {
		PWSTR friendlyName = new (nothrow) WCHAR[nameLength];
		PWSTR manName = new (nothrow) WCHAR[manNameLength];

		if (friendlyName != nullptr && manName != nullptr) {
			ZeroMemory(friendlyName, nameLength * sizeof(WCHAR));

			hName = devMgr->GetDeviceFriendlyName(id, friendlyName, &nameLength);
			hMan = devMgr->GetDeviceManufacturer(id, manName, &manNameLength);

			if (SUCCEEDED(hName) && SUCCEEDED(hMan)) {
				name << friendlyName;
				manufacturer << manName;
			}
			else {
				return make_tuple("", "");
			}
			delete[] friendlyName;
			delete[] manName;
		}
		else {
			throw exception("Can't allocate memory.");
		}
	}
	else {
		return make_tuple("", "");
	}

	string multiByteName = wstring_convert<
			codecvt_utf8<wchar_t>>().to_bytes(name.str());
	string multiByteMan = wstring_convert<
			codecvt_utf8<wchar_t>>().to_bytes(manufacturer.str());

	return make_tuple(multiByteName, multiByteMan);
}

void DeviceScanner::listMtpDevices() {
	HRESULT hRes = CoInitialize(NULL);
	LPWSTR* devListArray = NULL;
	DWORD   cdwDevices = 0;
	IPortableDeviceManager* spDevMgr;

	clearMtpList();

	hRes = CoCreateInstance(CLSID_PortableDeviceManager, NULL,
		CLSCTX_INPROC_SERVER, IID_IPortableDeviceManager,
		(VOID**)&spDevMgr);

	hRes = spDevMgr->GetDevices(NULL, &cdwDevices);
	devListArray = new LPWSTR[cdwDevices];
	hRes = spDevMgr->GetDevices(devListArray, &cdwDevices);

	if (SUCCEEDED(hRes)) {
		for (DWORD dwIndex = 0; dwIndex < cdwDevices; dwIndex++) {
			tuple<string, string> temp = identifyMtpDevice(spDevMgr,
					devListArray[dwIndex]);
				if (isDriveExist(get<0>(temp)) == false) {
					mtp_dev.push_back(new MtpDevice(get<0>(temp), get<1>(temp)));
				}
			}
		}
		else {
			throw exception("Can't list WPD (MTP) devices.");
		}
	}

void DeviceScanner::listUsbDevices() {
	string disk_letter;
	DWORD dwMask;
	DWORD dwDriveMask = GetLogicalDrives();

	clearUsbList();

	for (auto i = 0; i < 26; i++) {
		disk_letter.clear();
		dwMask = (dwDriveMask >> i) & 0x00000001;
		if (dwMask == 1) {
			disk_letter.push_back(static_cast<char>(65 + i));
			disk_letter.append(":\\");

			if (isRemovable(disk_letter)) {
				usb_dev.push_back(new UsbDevice(disk_letter));
			}
		}
	}
}

bool DeviceScanner::isRemovable(string dev) {
	return GetDriveType(dev.c_str()) == DRIVE_REMOVABLE ? true : false;
}

bool DeviceScanner::ejectDevice(string name) {
	bool result = false;

	for (auto i = 0; i < usb_dev.size(); i++) {
		if (usb_dev.at(i)->getLetter() == name) {
			int check = usb_dev.at(i)->badlyRemove();
			if (check == 0) {
				return false;
			}
			delete usb_dev.at(i);

			usb_dev.erase(usb_dev.begin() + i);
			result = true;
			break;
		}
	}
	return result;
}

bool DeviceScanner::ejectDeviceS(string name) {
	bool result = false;

	for (auto i = 0; i < usb_dev.size(); i++) {
		if (usb_dev.at(i)->getLetter() == name) {
			int check = usb_dev.at(i)->safetyRemove();
			if (check == 0) {
				return false;
			}
			delete usb_dev.at(i);

			usb_dev.erase(usb_dev.begin() + i);
			result = true;
			break;
		}
	}
	return result;
}

bool DeviceScanner::lockdevice(string name) {
	bool result = false;

	for (auto i = 0; i < usb_dev.size(); i++) {
		if (usb_dev.at(i)->getLetter() == name) {
			int check = usb_dev.at(i)->safetyRemove();
			if (check == 0) {
				return false;
			}
			delete usb_dev.at(i);

			usb_dev.erase(usb_dev.begin() + i);
			result = true;
			break;
		}
	}
	return result;
}

bool DeviceScanner::notifyChange() {
	if (prev_mtp != mtp_dev.size() || prev_usb != usb_dev.size()) {
		prev_mtp = mtp_dev.size();
		prev_usb = usb_dev.size();
		return true;
	}
	else {
		return false;
	}
}

bool DeviceScanner::isDriveExist(string data) {
	int count = 0;

	for (auto entry : usb_dev) {
		if (entry->getName() == data) {
			count++;
		}
	}
	return count != 0 ? true : false;
}

void DeviceScanner::printDrives() {
	cout << "Founded " << mtp_dev.size()
		<< " WPD (MTP) devices:" << endl << endl;
	if (mtp_dev.size() != 0) {
		for (auto entry : mtp_dev) {
			entry->showInfo();
			cout << endl;
		}
	}

	cout << "Founded " << usb_dev.size()
		<< " Mass storage devices: " << endl << endl;
	if (usb_dev.size() != 0) {
		for (auto entry : usb_dev) {
			entry->showInfo();
			cout << endl;
		}
	}
}