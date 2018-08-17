#include <windows.h>
#include <sysinfoapi.h>
#include <setupapi.h>
#include <hidsdi.h>
#include <hidclass.h>
#include "hid.h"

static HANDLE rx_event = NULL;
static HANDLE tx_event = NULL;
static CRITICAL_SECTION rx_mutex;
static CRITICAL_SECTION tx_mutex;

static int hid_rx(HANDLE handle, void *buf, int len, int timeout)
{
	unsigned char tmpbuf[516];
	OVERLAPPED ov;
	DWORD n, r;

	if (sizeof(tmpbuf) < len + 1)
		return -1;

	EnterCriticalSection(&rx_mutex);
	ResetEvent(&rx_event);

	memset(&ov, 0, sizeof(ov));
	ov.hEvent = rx_event;

	if (!ReadFile(handle, tmpbuf, len + 1, NULL, &ov))
	{
		if (GetLastError() != ERROR_IO_PENDING)
			goto return_error;

		r = WaitForSingleObject(rx_event, timeout);

		if (r == WAIT_TIMEOUT)
			goto return_timeout;

		if (r != WAIT_OBJECT_0)
			goto return_error;
	}

	if (!GetOverlappedResult(handle, &ov, &n, FALSE))
		goto return_error;

	LeaveCriticalSection(&rx_mutex);

	if (n <= 0)
		return -1;
	n--;

	if (n > len) n = len;

	memcpy(buf, tmpbuf + 1, n);

	return n;

return_timeout:
	CancelIo(handle);
	LeaveCriticalSection(&rx_mutex);
	return 0;

return_error:
	LeaveCriticalSection(&rx_mutex);
	return -1;
}

static int hid_tx(HANDLE handle, void *buf, int len, int timeout)
{
	unsigned char tmpbuf[516];
	OVERLAPPED ov;
	DWORD n, r;

	if (sizeof(tmpbuf) < len + 1)
		return -1;

	EnterCriticalSection(&tx_mutex);

	ResetEvent(&tx_event);

	memset(&ov, 0, sizeof(ov));

	ov.hEvent = tx_event;

	tmpbuf[0] = 0;
	memcpy(tmpbuf + 1, buf, len);

	if (!WriteFile(handle, tmpbuf, len + 1, NULL, &ov))
	{
		if (GetLastError() != ERROR_IO_PENDING)
			goto return_error;

		r = WaitForSingleObject(tx_event, timeout);

		if (r == WAIT_TIMEOUT)
			goto return_timeout;

		if (r != WAIT_OBJECT_0)
			goto return_error;
	}

	if (!GetOverlappedResult(handle, &ov, &n, FALSE))
		goto return_error;

	LeaveCriticalSection(&tx_mutex);

	if (n <= 0)
		return -1;

	return n - 1;

return_timeout:
	CancelIo(handle);
	LeaveCriticalSection(&tx_mutex);
	return 0;

return_error:
	//    log(LOG_TYPE_ERROR, "Error TX\n", Qt::red);
	LeaveCriticalSection(&tx_mutex);
	return -1;
}

int open(HID* hid, unsigned int vid, unsigned int pid, unsigned int usage, unsigned int usage_page)
{
	GUID guid;
	HDEVINFO info;
	DWORD index = 0, reqd_size;
	SP_DEVICE_INTERFACE_DATA iface;
	SP_DEVICE_INTERFACE_DETAIL_DATA *details;
	HIDD_ATTRIBUTES attrib;
	PHIDP_PREPARSED_DATA hid_data;
	HIDP_CAPS capabilities;
	HANDLE h;
	BOOL ret;
	int count = 0;

	if (!rx_event)
	{
		rx_event = CreateEvent(NULL, TRUE, TRUE, NULL);
		tx_event = CreateEvent(NULL, TRUE, TRUE, NULL);
		InitializeCriticalSection(&rx_mutex);
		InitializeCriticalSection(&tx_mutex);
	}

	HidD_GetHidGuid(&guid);

	info = SetupDiGetClassDevs(&guid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

	if (info == INVALID_HANDLE_VALUE)
		return 0;

	for (index = 0; 1; index++)
	{
		iface.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
		ret = SetupDiEnumDeviceInterfaces(info, NULL, &guid, index, &iface);

		if (!ret)
			return -1;

		SetupDiGetInterfaceDeviceDetail(info, &iface, NULL, 0, &reqd_size, NULL);
		details = (SP_DEVICE_INTERFACE_DETAIL_DATA *)malloc(reqd_size);

		if (details == NULL)
			continue;

		memset(details, 0, reqd_size);
		details->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
		ret = SetupDiGetDeviceInterfaceDetail(info, &iface, details,
			reqd_size, NULL, NULL);

		if (!ret)
		{
			free(details);
			continue;
		}

		h = CreateFile(details->DevicePath, GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
			OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
		free(details);

		if (h == INVALID_HANDLE_VALUE)
			continue;

		attrib.Size = sizeof(HIDD_ATTRIBUTES);
		ret = HidD_GetAttributes(h, &attrib);

		if (!ret || (vid > 0 && attrib.VendorID != vid) ||
			(pid > 0 && attrib.ProductID != pid) ||
			!HidD_GetPreparsedData(h, &hid_data))
		{
			CloseHandle(h);
			continue;
		}

		if (!HidP_GetCaps(hid_data, &capabilities))
		{
			HidD_FreePreparsedData(hid_data);
			CloseHandle(h);
			continue;
		}

		if (capabilities.UsagePage != usage_page || capabilities.Usage != usage)
		{
			HidD_FreePreparsedData(hid_data);
			CloseHandle(h);
			continue;
		}

		HidD_FreePreparsedData(hid_data);

		hid->handle = h;
		hid->is_open = true;
		break;
	}

	return 0;
}

int data_exchange(HID* hid, uint8_t report[REPORT_SIZE], unsigned int timeout_ms)
{
	int res = -1;
	if (!hid->is_open)
		return res;
	/* send report */
	if (0 != (res = hid_tx(hid->handle, (void*)report, REPORT_SIZE, timeout_ms)))
		return res;
	/* get report */
	return hid_rx(hid->handle, (void*)report, REPORT_SIZE, timeout_ms);
}

int close(HID* hid)
{
	if (!hid->is_open)
		return -1;

	if (hid->handle)
		CloseHandle(hid->handle);

	hid->handle = NULL;
	hid->is_open = false;
	return 0;
}
