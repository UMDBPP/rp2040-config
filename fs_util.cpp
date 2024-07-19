#include <stdio.h>
#include <string.h>

#include "./ff15/source/diskio.h"
#include "./ff15/source/ff.h"
#include "MB85RS1MT.h"
#include "config.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "pico/stdlib.h"

static uint32_t init_flag = 0;

spi_inst_t* spi = spi0;
const uint sck_pin = 18;
const uint mosi_pin = 19;
const uint miso_pin = 20;
const uint cs_pin = 1;
const uint wp_pin = 9;
const uint hold_pin = 7;

MB85RS1MT mem(spi, cs_pin, sck_pin, mosi_pin, miso_pin, wp_pin, hold_pin);

DSTATUS disk_status(BYTE pdrv) {
    DSTATUS status = init_flag;

    return status;
}

DSTATUS disk_initialize(BYTE pdrv) {
    // SPI Settings
    spi_init(spi, 1000 * 1000);

    spi_set_format(spi,            // SPI instance
                   8,              // Number of bits per transfer
                   (spi_cpol_t)0,  // Polarity (CPOL)
                   (spi_cpha_t)0,  // Phase (CPHA)
                   SPI_MSB_FIRST);

    // Init memory device, in this case some FRAM from Adafruit
    mem.mem_init();

    printf("Device ID: %d\n", mem.device_id);

    return RES_OK;
}

DRESULT disk_read(BYTE pdrv, BYTE* buff, LBA_t sector, UINT count) {
    if (!(disk_status(pdrv) && 0x01)) return RES_NOTRDY;

    if (FF_MIN_SS == FF_MAX_SS) {
        mem.read_memory(sector * FF_MIN_SS, (uint8_t*)buff, count * FF_MIN_SS);
    } else {
        printf("Min/Max sector size not equal");
        return RES_PARERR;
    }

    return RES_OK;
}
DRESULT disk_write(BYTE pdrv, const BYTE* buff, LBA_t sector, UINT count) {
    if (!(disk_status(pdrv) && 0x01)) return RES_NOTRDY;

    if (FF_MIN_SS == FF_MAX_SS) {
        mem.write_memory(sector * FF_MIN_SS, (uint8_t*)buff, count * FF_MIN_SS);
    } else {
        printf("Min/Max sector size not equal");
        return RES_PARERR;
    }

    return RES_OK;
}
DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void* buff) {
    if (cmd != CTRL_SYNC) {
        printf("variable sector size not implemented");
        return RES_ERROR;
    } else {
        // nothing to do here since the SPI writes are blocking
        return RES_OK;
    }
}

DWORD get_fattime(void) {
    const DWORD timestamp =
        0x2E290000;  // forgive the magic number, since there's no reliable time
                     // keeping every file will just get this time stamp which
                     // is January 9th, 2003

    return timestamp;
}