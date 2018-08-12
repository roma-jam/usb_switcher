/*
 * device.h
 *
 *  Created on: 12 рту. 2018 у.
 *      Author: RomaJam
 */

#ifndef DEVICE_H_
#define DEVICE_H_

#include "app.h"

typedef enum
{
    DEVICE_STATE_UNAWARE = 0,
    DEVICE_STATE_OFF,
    DEVICE_STATE_ON
} DEVICE_STATE;

typedef struct {
    DEVICE_STATE state;
    unsigned int dm_dp_delay_ms;
} DEVICE;

void device_init(APP* app);
void device_set_state(APP* app, DEVICE_STATE new_state);


#endif /* DEVICE_H_ */
