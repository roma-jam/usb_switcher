/*
 * app_hid.h
 *
 *  Created on: 15 рту. 2018 у.
 *      Author: RLeonov
 */

#ifndef APP_HID_H_
#define APP_HID_H_

#include "app.h"
#include "rexos/userspace/ipc.h"

typedef enum {
    HID_CMD_RESERVED = 0x00,
    HID_CMD_GET_INFO,
    HID_CMD_SET_STATE,
    HID_CMD_SET_CONFIG,
    HID_CMD_MAX
} HID_CMD;

#pragma pack(push, 1)
typedef struct {
    uint8_t cmd_id;
    uint8_t param1;
} HID_DATA_HEADER;

typedef struct {
    bool flag;
    uint32_t delay_ms;
    uint32_t timeout_ms;
} HID_CONFIG_DATA;

#pragma pack(pop)

void app_hid_request(APP* app, IPC* ipc);


#endif /* APP_HID_H_ */
