#include <PortableDevice.h>
#include <PortableDeviceApi.h>

#pragma comment(lib, "PortableDeviceGUIDs.lib")

#include <string>
#include <vector>

#include "MtpDevice.h"
#include "UsbDevice.h"

using namespace std;

class DeviceScanner {
	public:
		DeviceScanner();
		~DeviceScanner();

		void listUsbDevices();
		void listMtpDevices();

		tuple<string, string> identifyMtpDevice(_In_ IPortableDeviceManager* devMgr, _In_ PCWSTR id);

		bool isRemovable(string);
		bool isDriveExist(string);

		bool notifyChange();

		bool ejectDevice(string);
		bool ejectDeviceS(string);
		bool lockdevice(string);

		void clearUsbList();
		void clearMtpList();

		void printDrives();

	private:
		vector<UsbDevice*> usb_dev;
		vector<MtpDevice*> mtp_dev;

		size_t prev_usb;
		size_t prev_mtp;
};