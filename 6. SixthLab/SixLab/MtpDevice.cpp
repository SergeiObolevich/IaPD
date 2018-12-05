#include <iostream>
#include "MtpDevice.h"

MtpDevice::MtpDevice(std::string name, std::string manufacturer) {
	this->name = name;
	this->manufacturer = manufacturer;
}

MtpDevice::~MtpDevice() {}

void MtpDevice::showInfo() {
	cout << "Manufacturer: " << manufacturer << endl;
	cout << "Name        : " << name << endl;
}