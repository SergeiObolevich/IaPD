#include <iostream>
#include <iomanip>
#include <Windows.h>
#include <WinIoCtl.h>
#include <ntddscsi.h>
#include <conio.h>

using namespace std;

#define THOUSAND 1024
#define HUNDRED 100
#define BYTE_SIZE 8

string busType[] = {
	"UNKNOWN",
	"SCSI",
	"ATAPI",
	"ATA",
	"ONE_TREE_NINE_FOUR",
	"SSA",
	"FIBRE",
	"USB",
	"RAID",
	"ISCSI",
	"SAS",
	"SATA",
	"SD",
	"MMC"
};