/*
    RExOS - embedded RTOS
    Copyright (c) 2011-2016, Alexey Kramarenko
    All rights reserved.
*/

#include "rexos/userspace/stdio.h"
#include "rexos/userspace/stdlib.h"
#include "rexos/userspace/process.h"
#include "rexos/userspace/sys.h"
#include "rexos/userspace/stm32/stm32_driver.h"
#include "rexos/userspace/ipc.h"
#include "rexos/userspace/uart.h"
#include "rexos/userspace/process.h"
#include "rexos/userspace/pin.h"
#include "rexos/userspace/gpio.h"
#include "app_private.h"
#include "app_usb.h"
#include "device.h"
#include "config.h"
#include "led.h"

void app();

const REX __APP = {
    //name
    "App main",
    //size
    900,
    //priority
    200,
    //flags
    PROCESS_FLAGS_ACTIVE | REX_FLAG_PERSISTENT_NAME,
    //function
    app
};


static inline void app_setup_dbg()
{
    BAUD baudrate;
    pin_enable(DBG_CONSOLE_TX_PIN, STM32_GPIO_MODE_AF, AF4);
    uart_open(DBG_CONSOLE, UART_MODE_STREAM | UART_TX_STREAM);
    baudrate.baud = DBG_CONSOLE_BAUD;
    baudrate.data_bits = 8;
    baudrate.parity = 'N';
    baudrate.stop_bits= 1;
    uart_set_baudrate(DBG_CONSOLE, &baudrate);
    uart_setup_printk(DBG_CONSOLE);
    uart_setup_stdout(DBG_CONSOLE);
    open_stdout();
}

static inline void app_init(APP* app)
{
#if (APP_DEBUG)
    app_setup_dbg();
    printf("USB Switcher, CPU %d MHz\n", power_get_core_clock()/1000000);
#endif
}

void app()
{
    APP app;
    IPC ipc;

    app_init(&app);
    device_init(&app);
    led_init(&app);
    app_usb_init(&app);

    sleep_ms(100);
    process_info();

   app.timer = timer_create(0, HAL_APP);
//   timer_start_ms(app.timer, DEVICE_TEST_TIMEOUT_MS);

    for (;;)
    {
        ipc_read(&ipc);
        switch (HAL_GROUP(ipc.cmd))
        {
        case HAL_APP:
            if(HAL_ITEM(ipc.cmd) == IPC_TIMEOUT)
            {
                device_set_state(&app, (app.device.state == DEVICE_STATE_OFF)? DEVICE_STATE_ON : DEVICE_STATE_OFF);
                timer_start_ms(app.timer, DEVICE_TEST_TIMEOUT_MS);
            }
            break;
        case HAL_USBD:
//            app_usb_request(&app, &ipc);
            break;
        case HAL_USBD_IFACE:
//            app_ccid_request(&app, &ipc);
            break;
        default:
            error(ERROR_NOT_SUPPORTED);
            break;
        }
        ipc_write(&ipc);
    }
}
