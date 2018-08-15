#ifndef HID_H
#define HID_H

#include <QWidget>
#include <QObject>
#include <stdint.h>
#include <windows.h>
#include "config.h"

typedef enum {
    MEMORY_STATUS_OK = 0x00,
    MEMORY_STATUS_ERROR,
    MEMORY_STATUS_NOT_ERASED,
    MEMORY_STATUS_WRONG_LENGTH,
    MEMORY_STATUS_WRONG_OFFSET,
} MEMORY_STATUS;

#pragma pack(push, 1)
typedef struct {
    uint8_t tag;
    uint8_t size;
    uint8_t pdata;
} MEMORY_IN_REPORT;

typedef struct {
    uint8_t tag;
    uint8_t status;
    uint8_t size;
    uint8_t pdata;
} MEMORY_OUT_REPORT;

typedef struct {
    uint32_t label;
    uint32_t addr;
    uint32_t size;
    uint32_t crc32;
    uint8_t product_key[32];
    uint8_t data[80];
} BOOT_HEADER;
#pragma pack(pop)

class hid_t : public QObject {
    Q_OBJECT
signals:
//    void log(LOG_TYPE type, const QString& text, const QColor& color);
    void device_founded(QString,QString,QString);

protected:
//    void info(const QString& text, const QColor& color = Qt::black) {log(LOG_TYPE_DEFAULT, text, color);}


private:
    HANDLE handle;
    bool opened;

    int hid_rx(void *buf, int len, int timeout);
    int hid_tx(void *buf, int len, int timeout);

public:
    hid_t(QWidget *parent = 0);
    ~hid_t();

    int search_devices();
    int open(unsigned int vid, unsigned int pid);
    int close();
    int set_state(bool active);
    int get_info(DEVICE* device);
    bool is_open() { return this->opened; }
};

#endif // HID_H
#ifndef HID_H
#define HID_H

#endif // HID_H
