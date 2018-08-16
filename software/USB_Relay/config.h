#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

#define TITLE                   "USB Relay v.0.0.3b"

#define VID                     0x24DC
#define PID                     0x0FFE
#define USAGE_PAGE              0xFA1D
#define USAGE                   0x0002

#define REPORT_SIZE             64

#define WRITE_CHUNK_SIZE        55

typedef enum {
    DEVICE_STATE_UNAWARE = 0,
    DEVICE_STATE_OFF,
    DEVICE_STATE_ON
} DEVICE_STATE;

#pragma pack(push, 1)
typedef struct {
    uint32_t magic;
    bool standalone_flag;
    uint8_t state;
    uint32_t delay_ms;
    uint32_t timeout_ms;
    uint32_t switch_counter;
    uint32_t crc32;
} DEVICE;
#pragma pack(pop)


#endif // CONFIG_H
