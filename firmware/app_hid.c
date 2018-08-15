/*
 * app_hid.c
 *
 *  Created on: 15 рту. 2018 у.
 *      Author: RLeonov
 */

#include <string.h>
#include "app_private.h"
#include "rexos/userspace/stdio.h"
#include "rexos/userspace/types.h"
#include "rexos/userspace/error.h"
#include "rexos/userspace/usb.h"
#include "rexos/userspace/hid.h"
#include "rexos/userspace/io.h"
#include "app_hid.h"
#include "config.h"
#include "device.h"

static inline void app_hid_complete(APP* app, IPC* ipc)
{
#if (APP_DEBUG_HID)
    printf("APP HID: Complete\n");
#endif // APP_DEBUG_HID

    /* proceed communication */
    ipc_post_inline(app->usbd, HAL_CMD(HAL_USBD_IFACE, USB_HID_OUT), USBD_IFACE(USB_HID_INTERFACE, 0), 0, 0);
}

static inline void app_hid_process_data(APP* app, IPC* ipc)
{
#if (APP_DEBUG_HID)
    printf("APP HID: Out\n");
#endif // APP_DEBUG_HID
    IO* io = (IO*)ipc->param2;
    DATA_HEADER* data_header = (DATA_HEADER*)io_data(io);

    switch (data_header->cmd_id)
    {
        case HID_CMD_GET_INFO:
            data_header->param1 = sizeof(DEVICE);
            memcpy((uint8_t*)(io_data(io) + sizeof(DATA_HEADER)),
                    (uint8_t*)&app->device,
                    sizeof(DEVICE));
            /* TODO: */
            break;
        case HID_CMD_SET_STATE:
            if(data_header->param1)
                device_set_state(app, DEVICE_STATE_ON);
            else
                device_set_state(app, DEVICE_STATE_OFF);
            break;
        case HID_CMD_SET_CONFIG:
            device_set_config(app,
                    (bool)(io_data(io) + sizeof(DATA_HEADER)),
                    (unsigned int)(io_data(io) + sizeof(DATA_HEADER) + sizeof(bool)));
            break;
        default:
            break;
    }

    ipc_post_inline(app->usbd,
            HAL_CMD(HAL_USBD_IFACE, USB_HID_IN),
            USBD_IFACE(USB_HID_INTERFACE, 0),
            0,
            0);
}

void app_hid_request(APP* app, IPC* ipc)
{
    switch (HAL_ITEM(ipc->cmd))
    {
        case USB_HID_OUT:
            app_hid_process_data(app, ipc);
            break;
        case USB_HID_IN:
            app_hid_complete(app, ipc);
            break;
        default:
            error(ERROR_NOT_SUPPORTED);
            break;
    }
}
