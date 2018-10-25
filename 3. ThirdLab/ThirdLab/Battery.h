#pragma once 
#include <Windows.h>
#include <iostream>
#include <conio.h>
#include <cstdlib>

#include "PowerStatus.h"
#include "ChargeStatus.h"

using namespace std;

const unsigned int MAX_VALUE = 4294967295;

class Battery {
private:
	PowerStatus powerStatus;
	ChargeStatus chargeStatus;
	unsigned int lifePercentage;
	DWORD lifetime;
	DWORD fullLifetime;
	bool saverStatus; 
public:
	friend ostream& operator<<(ostream&, const Battery&);

	Battery() {this->UpdateStatus();}

	~Battery() {}

	void UpdateStatus() {
		SYSTEM_POWER_STATUS status;

		if (!GetSystemPowerStatus(&status)) {
			throw system_error(error_code(GetLastError(), system_category()), "Retreiving status error.");
		}

		this->powerStatus = (PowerStatus)status.ACLineStatus;
		this->chargeStatus = (ChargeStatus)status.BatteryFlag;
		this->lifetime = status.BatteryLifeTime;
		this->lifePercentage = (unsigned int)(status.BatteryLifePercent);
		this->fullLifetime = (unsigned int)(status.BatteryFullLifeTime);
		this->saverStatus = (unsigned int)status.SystemStatusFlag == 1;
	}
};

ostream& operator<<(ostream& stream, const Battery& battery) {
	stream << "******************************" << endl;
	stream << " Power status: ";
	stream << GetPowerStatus(battery.powerStatus);
	stream << endl << "******************************" << endl;

	stream << "******************************" << endl;
	stream << " Charge status: ";
	stream << GetChargeStatus(battery.chargeStatus);
	stream << endl << "******************************" << endl;

	stream << "******************************" << endl;
	stream << " Life percentage: ";
	if (battery.lifePercentage == 255) {
		stream << "Unknown";
	}
	else {
		stream << battery.lifePercentage << "%";
	}
	stream << endl << "******************************" << endl;

	stream << "******************************" << endl;
	stream << " Lifetime: ";
	if (battery.lifetime == MAX_VALUE) {
		stream << "Invalid time";
	}
	else {
		stream << battery.lifetime / 60 << " minutes";
	}
	stream << endl << "******************************" << endl;

	stream << "******************************" << endl;
	stream << " Full lifetime: ";
	if (battery.fullLifetime == MAX_VALUE) {
		stream << "Invalid time";
	}
	else {
		stream << battery.fullLifetime / 60 << " minutes";
	}
	stream << endl << "******************************" << endl;

	stream << "******************************" << endl;
	stream << " Saver status: ";
	stream << std::boolalpha << battery.saverStatus;
	stream << endl << "******************************" << endl;

	return stream;
}