// usb_reley.cpp : Defines the exported functions for the DLL application.
//
#ifdef WIN32
#include <windows.h>
#endif
#include <stdint.h>
#include <stdbool.h>

#include "usb_relay.h"
#include "hid.h"

typedef enum {
	HID_CMD_RESERVED = 0x00,
	HID_CMD_GET_INFO,
	HID_CMD_SET_STATE,
	HID_CMD_SET_CONFIG,
	HID_CMD_START_UPDATE_FIRMWARE,
	HID_CMD_UPDATE_FIRMWARE,
	HID_CMD_LAUNCH_FIRMWARE,
	HID_CMD_MAX
} HID_CMD;


USBRELAYDLL_API void* __cdecl usb_relay_open(unsigned int vid, unsigned int pid, unsigned int usage, unsigned int usage_page)
{
	HID* handle = (HID*)malloc(sizeof(HID));
	/* open hid device by vid pid, create its handle */
	open(handle, vid, pid, usage, usage_page);
	/* return it */
	return (void*)handle;
}

USBRELAYDLL_API	int __cdecl usb_relay_close(void* handle)
{
	/* close hid device by handle */
	close(handle);
	free(handle);
	return 0;
}

USBRELAYDLL_API int __cdecl usb_relay_get_status(void* handle, USB_RELAY_CONFIG* cofnig, unsigned int timeout_ms)
{
	uint8_t report[REPORT_SIZE] = { 0 };
	/* get status of hid device by handle */
	report[0] = HID_CMD_GET_INFO;
	data_exchange((HID*)handle, report, timeout_ms);
	return 0;
}

USBRELAYDLL_API int __cdecl usb_relay_set_state(void* handle, USB_RELAY_DEVICE_STATE state, unsigned int timeout_ms)
{
	uint8_t report[REPORT_SIZE] = { 0 };
	/* set state to device by handle */
	report[0] = HID_CMD_SET_STATE;
	report[1] = (state == USB_RELAY_DEVICE_STATE_ON) ? 0x01 : 0x00;
	data_exchange((HID*)handle, report, timeout_ms);
	return 0;
}