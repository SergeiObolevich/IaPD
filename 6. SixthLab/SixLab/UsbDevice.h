#include <Windows.h>
#include <winioctl.h>
#include <winusb.h>
#include <cstdint>
#include <string>

using namespace std;

static const uint32_t kilobytes = 1024;
static const uint32_t megabytes = kilobytes * kilobytes;

class UsbDevice {
	public:
		UsbDevice(string);
		~UsbDevice();

		int safetyRemove();
		int badlyRemove();
		void getDeviceInfo();
		void getDeviceSize();
		int lock();
		int lockDevice();
		void dismountDevice();
		void removeMedia();
		void ejectMedia();
		void showInfo();

		string getName();
		string getLetter();

	private:
		HANDLE hDrive;

		string letter;
		string link;
		string name;
		string file_sys;

		uint64_t full_space;
		uint64_t busy_space;
		uint64_t free_space;
};