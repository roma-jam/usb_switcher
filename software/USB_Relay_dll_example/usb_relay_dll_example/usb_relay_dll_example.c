// usb_relay_dll_example.cpp : Defines the entry point for the console application.
//
#include <stdio.h>
#include <windows.h>

#include "usb_relay.h"

p_usb_relay_open		relay_open;
p_usb_relay_close		relay_close;
p_usb_relay_get_status	relay_get_status;
p_usb_relay_set_state	relay_set_state;

static inline char get_key()
{
	char key = '0';
	do
	{
		key = getc(stdin);
		Sleep(250);
	}
	while (key != '\n' && getc(stdin) != '\n');
	return key;
}

int main(int argc, char** argv)
{
	printf("USB Relay DLL Usage Example, @RL, 2018\n\n");
	/* initialize instance */
	HINSTANCE	usb_relay_dll;

	printf("Import usb_relay.dll ...");

	if (NULL == (usb_relay_dll = LoadLibrary("usb_relay.dll")))
	{
		printf("FAILURE\n");
		printf("ERROR: %#X\n", GetLastError());
		get_key();
		return -1;
	}
	printf("OK\n");

	printf("Extract \"usb_relay_open\" function ... ");
	relay_open = (p_usb_relay_open)GetProcAddress(usb_relay_dll, "usb_relay_open");
	if (relay_open == NULL)
	{
		printf("FAILURE\n");
		printf("ERROR: %#X\n", GetLastError());
		get_key();
		FreeLibrary(usb_relay_dll);
		return -1;
	}
	printf("OK\n");

	printf("Extract \"usb_relay_close\" function ... ");
	relay_close = (p_usb_relay_close)GetProcAddress(usb_relay_dll, "usb_relay_close");
	if (relay_close == NULL)
	{
		printf("FAILURE\n");
		printf("ERROR: %#X\n", GetLastError());
		get_key();
		FreeLibrary(usb_relay_dll);
		return -1;
	}
	printf("OK\n");

	printf("Extract \"usb_relay_get_status\" function ... ");
	relay_get_status = (p_usb_relay_get_status)GetProcAddress(usb_relay_dll, "usb_relay_get_status");
	if (relay_get_status == NULL)
	{
		printf("FAILURE\n");
		printf("ERROR: %#X\n", GetLastError());
		get_key();
		FreeLibrary(usb_relay_dll);
		return -1;
	}
	printf("OK\n");

	printf("Extract \"usb_relay_set_state\" function ... ");
	relay_set_state = (p_usb_relay_set_state)GetProcAddress(usb_relay_dll, "usb_relay_set_state");
	if (relay_set_state == NULL)
	{
		printf("FAILURE\n");
		printf("ERROR: %#X\n", GetLastError());
		get_key();
		FreeLibrary(usb_relay_dll);
		return -1;
	}
	printf("OK\n\n");

	/* DEVICE COMMUNICATION */

	/* open device */
	printf("Device open ... ");
	void* device = relay_open(0x24DC, 0x0FFE, 0x0002, 0xFA1D);
	if (device == NULL)
	{
		printf("FAILURE\n");
		printf("ERROR: %#X\n", GetLastError());
		get_key();
		FreeLibrary(usb_relay_dll);
		return -1;
	}
	printf("OK\n");

	/* delay 5 SEC */
	printf("Wait 5 sec ...\n");
	Sleep(5000);

	/* set device ON */
	printf("Device set state ON ... ");
	relay_set_state(device, USB_RELAY_DEVICE_STATE_ON, 1000);
	printf("OK\n");

	/* delay 5 SEC */
	printf("Wait 5 sec ...\n");
	Sleep(5000);

	/* set device OFF */
	printf("Device set state OFF ... ");
	relay_set_state(device, USB_RELAY_DEVICE_STATE_OFF, 1000);
	printf("OK\n");

	/* close device */
	printf("Device close ... ");
	relay_close(device);
	printf("OK\n");
	/* ******************** */

	FreeLibrary(usb_relay_dll);

	get_key();
    return 0;
}

