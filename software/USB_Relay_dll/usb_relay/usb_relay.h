#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __GNUC__
#include <stdlib.h>
#include <string.h>
#define __stdcall
#define __cdecl
#endif

#ifdef USBRELAYDLL_EXPORTS
	#define USBRELAYDLL_API __declspec(dllexport) 
#else
	#define USBRELAYDLL_API __declspec(dllimport) 
#endif

#ifndef USBRELAYDLL_API
	#define USBRELAYDLL_API
#endif

typedef enum {
	USB_RELAY_DEVICE_STATE_OFF = 0,
	USB_RELAY_DEVICE_STATE_ON
} USB_RELAY_DEVICE_STATE;

#pragma pack(push, 1)
typedef struct {
	uint32_t magic;
	bool standalone_flag;
	uint8_t state;
	uint32_t delay_ms;
	uint32_t timeout_ms;
	uint32_t switch_counter;
	uint32_t crc32;
} USB_RELAY_CONFIG;
#pragma pack(pop)

#ifdef __cplusplus
extern "C"
{
#endif

	USBRELAYDLL_API	void* __cdecl usb_relay_open(unsigned int vid, unsigned int pid, unsigned int usage, unsigned int usage_page);

	USBRELAYDLL_API	int __cdecl usb_relay_close(void* handle);

	USBRELAYDLL_API int __cdecl usb_relay_get_status(void* handle, USB_RELAY_CONFIG* cofnig, unsigned int timeout_ms);

	USBRELAYDLL_API int __cdecl usb_relay_set_state(void* handle, USB_RELAY_DEVICE_STATE state, unsigned int timeout_ms);

#ifdef __cplusplus
}

typedef void*(__cdecl *p_usb_relay_open)(unsigned int vid, unsigned int pid, unsigned int usage, unsigned int usage_page);

typedef int(__cdecl *p_usb_relay_close)(void* handle);

typedef int(__cdecl *p_usb_relay_get_status)(void* handle, USB_RELAY_CONFIG* cofnig, unsigned int timeout_ms);

typedef int(__cdecl *p_usb_relay_set_state)(void* handle, USB_RELAY_DEVICE_STATE state, unsigned int timeout_ms);

#endif
