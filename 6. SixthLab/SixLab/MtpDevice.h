#include <string>

using namespace std;

class MtpDevice {
	public:
		MtpDevice(string name, string manufacturer);
		~MtpDevice();
		void showInfo();
	private:
		string name;
		string manufacturer;
};