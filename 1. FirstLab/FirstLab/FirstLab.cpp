#include "FirstLab.h"

int main() {
	int deviceIndex = 0;

	DWORD propertyType = 0;
	DWORD requiredSize = 0;

	SP_DEVINFO_DATA sp_devinfo_data;

	char propertyBuffer[256];

	setlocale(LC_ALL, "Russian");

	HDEVINFO deviceInfo = SetupDiGetClassDevs(0, "PCI", NULL, DIGCF_ALLCLASSES | DIGCF_PRESENT);
	if (deviceInfo == INVALID_HANDLE_VALUE) {
		printf("Не удается получить список устройств.\n");
		getchar();
		system("pause");
		return EXIT_FAILURE;
	}

	sp_devinfo_data.cbSize = sizeof(sp_devinfo_data);

	printf(" # | Производитель\t\t\t    | Имя\n\n");

	while (SetupDiEnumDeviceInfo(deviceInfo, deviceIndex, &sp_devinfo_data)) {
		SetupDiGetDeviceRegistryProperty(deviceInfo, &sp_devinfo_data, SPDRP_MFG, &propertyType, (BYTE*)propertyBuffer, 256, &requiredSize);
		printf("#%-2d| %-39s| ", deviceIndex + 1, propertyBuffer);

		SetupDiGetDeviceRegistryProperty(deviceInfo, &sp_devinfo_data, SPDRP_DEVICEDESC, &propertyType, (BYTE*)propertyBuffer, 256, &requiredSize);
		printf("%s\n", propertyBuffer);

		deviceIndex++;
	}
	
	getchar();
	system("pause");
	return EXIT_SUCCESS;
}