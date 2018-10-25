#pragma once
#include <string>

using namespace std;

enum PowerStatus {
	Offline = 0,
	Online = 1,
	UnknownStatus = 255
};

string GetPowerStatus(int status) {
	switch (status) {
		case PowerStatus::Offline: {return string("Offline");}
		case PowerStatus::Online: {return string("Online");}
		case PowerStatus::UnknownStatus: {return string("Unknown");}
		default: return string("Invalid status");
	}
}