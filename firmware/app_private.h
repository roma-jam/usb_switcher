/*
    RExOS - embedded RTOS
    Copyright (c) 2011-2016, Alexey Kramarenko
    All rights reserved.
*/

#ifndef APP_PRIVATE_H
#define APP_PRIVATE_H

#include "app.h"
#include "rexos/userspace/types.h"
#include "rexos/userspace/ipc.h"
#include "device.h"

typedef struct _APP {
    DEVICE device;
    HANDLE timer;
    HANDLE usbd;
    bool usb_started;
} APP;

#endif // APP_PRIVATE_H
