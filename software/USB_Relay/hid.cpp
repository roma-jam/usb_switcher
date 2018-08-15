#include <QDebug>

#include <windows.h>
#include <sysinfoapi.h>
#include <setupapi.h>
#include <hidsdi.h>
#include <hidclass.h>
#include "hid.h"
#include "config.h"

static HANDLE rx_event = NULL;
static HANDLE tx_event = NULL;
static CRITICAL_SECTION rx_mutex;
static CRITICAL_SECTION tx_mutex;


const unsigned char __GET_INFO[2] = {
    0x01, 0x00
};

const unsigned char __SET_STATE[2] = {
    0x02, 0x00
};

int get_tick_ms()
{
    return (int)GetTickCount();
}

void flip(uint8_t *data, uint32_t data_size)
{
    uint8_t byte = 0;
    for (uint32_t i = 0; i < (data_size >> 1); i++)
    {
        byte = data[i];
        data[i] = data[data_size - 1 - i];
        data[data_size - 1 - i] = byte;
    }
}

hid_t::hid_t(QWidget* parent)
{
    // constructor
    opened = false;
    handle = NULL;
}

hid_t::~hid_t()
{
    // destructor
}

//  rawhid_recv - receive a packet
//    Inputs:
//	num = device to receive from (zero based)
//	buf = buffer to receive packet
//	len = buffer's size
//	timeout = time to wait, in milliseconds
//    Output:
//	number of bytes received, or -1 on error
//

int hid_t::hid_rx(void *buf, int len, int timeout)
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
//    log(LOG_TYPE_ERROR, "Error RX\n", Qt::red);
    LeaveCriticalSection(&rx_mutex);
    return -1;
}

//  rawhid_send - send a packet
//    Inputs:
//	num = device to transmit to (zero based)
//	buf = buffer containing packet to send
//	len = number of bytes to transmit
//	timeout = time to wait, in milliseconds
//    Output:
//	number of bytes sent, or -1 on error
//
int hid_t::hid_tx(void *buf, int len, int timeout)
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


int hid_t::search_devices()
{
    GUID guid;
    HDEVINFO dev_info;
    DWORD index = 0, reqd_size;
    SP_DEVICE_INTERFACE_DATA iface;
    SP_DEVICE_INTERFACE_DETAIL_DATA *details;
    HIDD_ATTRIBUTES attrib;
    PHIDP_PREPARSED_DATA hid_data;
    HIDP_CAPS capabilities;
    HANDLE h;
    BOOL ret;
    wchar_t Name[20];
    int count = 0;

    if (!rx_event)
    {
        rx_event = CreateEvent(NULL, TRUE, TRUE, NULL);
        tx_event = CreateEvent(NULL, TRUE, TRUE, NULL);
        InitializeCriticalSection(&rx_mutex);
        InitializeCriticalSection(&tx_mutex);
    }

    HidD_GetHidGuid(&guid);

    dev_info = SetupDiGetClassDevs(&guid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

    if (dev_info == INVALID_HANDLE_VALUE)
        return 0;

    for (index = 0; 1; index++)
    {
        iface.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
        ret = SetupDiEnumDeviceInterfaces(dev_info, NULL, &guid, index, &iface);

        if (!ret)
            return count;

        SetupDiGetInterfaceDeviceDetail(dev_info, &iface, NULL, 0, &reqd_size, NULL);
        details = (SP_DEVICE_INTERFACE_DETAIL_DATA *)malloc(reqd_size);

        if (details == NULL)
            continue;

        memset(details, 0, reqd_size);
        details->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
        ret = SetupDiGetDeviceInterfaceDetail(dev_info, &iface, details,
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



        if (!ret || (attrib.VendorID != VID) ||
            (attrib.ProductID != PID) ||
            !HidD_GetPreparsedData(h, &hid_data))
        {
            CloseHandle(h);
            continue;
        }

        if(!HidP_GetCaps(hid_data, &capabilities))
        {
            HidD_FreePreparsedData(hid_data);
            CloseHandle(h);
            continue;
        }

        if (capabilities.UsagePage != USAGE_PAGE || capabilities.Usage != USAGE)
        {
            HidD_FreePreparsedData(hid_data);
            CloseHandle(h);
            continue;
        }

        if(HidD_GetProductString(h, Name, sizeof(Name)))
        {
            emit device_founded(QString::number(attrib.VendorID, 16).toUpper().rightJustified(4, '0').right(4),
                                QString::number(attrib.ProductID, 16).toUpper().rightJustified(4, '0').right(4),
                                QString::fromWCharArray(Name));
        }
        else
        {
            emit device_founded(QString::number(attrib.VendorID, 16).toUpper().rightJustified(4, '0').right(4),
                                QString::number(attrib.ProductID, 16).toUpper().rightJustified(4, '0').right(4),
                                "-");
        }

        HidD_FreePreparsedData(hid_data);
        CloseHandle(h);
    }
    return 0;
}


int hid_t::open(unsigned int vid, unsigned int pid)
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
    hid_t *hid;
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
            return count;

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

        if(!HidP_GetCaps(hid_data, &capabilities))
        {
            HidD_FreePreparsedData(hid_data);
            CloseHandle(h);
            continue;
        }

        if (capabilities.UsagePage != USAGE_PAGE || capabilities.Usage != USAGE)
        {
            HidD_FreePreparsedData(hid_data);
            CloseHandle(h);
            continue;
        }

        HidD_FreePreparsedData(hid_data);

        handle = h;
        this->opened = true;
        break;
    }

    return 0;
}

int hid_t::close()
{
    if(!this->opened)
        return 0;

    if(handle)
        CloseHandle(handle);

    handle = NULL;
    this->opened = false;
    return 0;
}

int hid_t::set_state(bool active)
{
    unsigned char report[REPORT_SIZE] = {0};
    if(!this->opened)
        return 0;

    memcpy(report, __SET_STATE, sizeof(__SET_STATE));

    if(active)
        report[1] = 0x01;

    hid_tx((void*)report, REPORT_SIZE, 100);
    return hid_rx((void*)report, REPORT_SIZE, 1000);
}

int hid_t::get_info(DEVICE* device)
{
    unsigned char report[REPORT_SIZE] = {0};
    if(!this->opened)
        return 0;

    memcpy(report, __GET_INFO, sizeof(__GET_INFO));

    hid_tx((void*)report, REPORT_SIZE, 100);
    hid_rx((void*)report, REPORT_SIZE, 1000);

    if(report[1] == sizeof(DEVICE))
    {
        memcpy((uint8_t*)device, report + 2, report[1]);
        return 1;
    }

    return 0;
}
