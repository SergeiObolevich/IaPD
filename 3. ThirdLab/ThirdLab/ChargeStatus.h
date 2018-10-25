#pragma once
#include <string>

using namespace std;

enum ChargeStatus {
	Middle = 0,
	High = 1,
	Low = 2,
	Critical = 4,
	ChargingLow = 8,
	ChargingHigh = 9,
	NoSystemBattery = 128,
	UnknownChargeStatus = 255
};

string GetChargeStatus(int status) {
	switch (status) {
		case ChargeStatus::Middle: {return string("Middle");}
		case ChargeStatus::High: {return string("High");}
		case ChargeStatus::Low: {return string("Low");}
		case ChargeStatus::Critical: {return string("Critical");}
		case ChargeStatus::ChargingLow: {return string("Charging [0..66]"); }
		case ChargeStatus::ChargingHigh: {return string("Charging [66..100]");}
		case ChargeStatus::NoSystemBattery: {return string("No System Battery");}
		case ChargeStatus::UnknownChargeStatus: {return string("Unknown");}
		default: return string("Invalid status");
	}
}