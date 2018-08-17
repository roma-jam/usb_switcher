#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <windows.h>

#define REPORT_SIZE					64

#pragma pack(push, 1)
typedef struct {
	HANDLE	handle;
	bool	is_open;
} HID;	
#pragma pack(pop)

int open(HID* hid, unsigned int vid, unsigned int pid, unsigned int usage, unsigned int usage_page);
int data_exchange(HID* hid, uint8_t report[REPORT_SIZE], unsigned int timeout_ms);
int close(HID* hid);

