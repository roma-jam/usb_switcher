/*
 * device.c
 *
 *  Created on: 12 рту. 2018 у.
 *      Author: RomaJam
 */


#include "app_private.h"
#include "rexos/userspace/stm32/stm32_driver.h"
#include "rexos/userspace/gpio.h"
#include "rexos/userspace/pin.h"
#include "config.h"

#define DEVICE_POWER_OFF()          { gpio_set_pin(DEVICE_POWER_PIN1); gpio_reset_pin(DEVICE_POWER_PIN2); }
#define DEVICE_POWER_ON()           { gpio_reset_pin(DEVICE_POWER_PIN1); gpio_set_pin(DEVICE_POWER_PIN2); }

#define DEVICE_DATA_OFF()           { gpio_reset_pin(DEVICE_DATA_PIN1); gpio_set_pin(DEVICE_DATA_PIN2); }
#define DEVICE_DATA_ON()            { gpio_set_pin(DEVICE_DATA_PIN1); gpio_reset_pin(DEVICE_DATA_PIN2); }

void device_init(APP* app)
{
    gpio_enable_pin(DEVICE_POWER_PIN1, GPIO_MODE_OUT);
    gpio_enable_pin(DEVICE_POWER_PIN2, GPIO_MODE_OUT);
    gpio_enable_pin(DEVICE_DATA_PIN1, GPIO_MODE_OUT);
    gpio_enable_pin(DEVICE_DATA_PIN2, GPIO_MODE_OUT);

    app->device.state = DEVICE_STATE_UNAWARE;
    app->device.dm_dp_delay_ms = DEVICE_DEAFULT_DELAY_MS;
    device_set_state(app, DEVICE_STATE_OFF);
}

void device_set_state(APP* app, DEVICE_STATE new_state)
{
    if(new_state == app->device.state)
        return;

    switch(new_state)
    {
        case DEVICE_STATE_OFF:
        {
            DEVICE_DATA_OFF();
            sleep_ms(app->device.dm_dp_delay_ms);
            DEVICE_POWER_OFF();
            break;
        }
        case DEVICE_STATE_ON:
        {
            DEVICE_POWER_ON();
            sleep_ms(app->device.dm_dp_delay_ms);
            DEVICE_DATA_ON();
            break;
        }
        default:
            return;
    }
    app->device.state = new_state;
}
