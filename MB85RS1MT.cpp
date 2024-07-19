#include "MB85RS1MT.h"

#include <stdio.h>
#include <string.h>

#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "pico/stdlib.h"

#define CS_HIGH gpio_put(cs_pin, 1);
#define CS_LOW gpio_put(cs_pin, 0);

#define HOLD_HIGH gpio_put(hold_pin, 1);
#define HOLD_LOW gpio_put(hold_pin, 0);

#define WP_HIGH gpio_put(wp_pin, 1);
#define WP_LOW gpio_put(wp_pin, 0);

// SPI Op-Code definitions
const uint8_t wren = 0x06;   // Set write enable latch
const uint8_t wrdi = 0x04;   // Reset write enable latch
const uint8_t rdsr = 0x05;   // Read status register
const uint8_t wrsr = 0x01;   // Write status register
const uint8_t read = 0x03;   // Read memory code
const uint8_t write = 0x02;  // Write memory code
const uint8_t rdid = 0x9F;   // Read device ID

#define REV16(x) \
    ((0x00FF & x) << 8) | ((0xFF00 & x) >> 8)  // for 16-bit numbers

#define REV24(x) \
    ((0x000000FF & x) << 16) | ((0x00FF0000 & x) >> 16)  // for 24-bit numbers

int MB85RS1MT::set_wel(void) {
    CS_LOW;
    spi_write_blocking(spi, &wren, 1);
    CS_HIGH;

    return 0;
}

int MB85RS1MT::reset_wel() {
    CS_LOW;
    spi_write_blocking(spi, &wrdi, 1);
    CS_HIGH;

    return 0;
}

int MB85RS1MT::read_status_register() {
    uint8_t buf = 0x01;

    CS_LOW;
    spi_write_blocking(spi, &rdsr, 1);
    spi_read_blocking(spi, 0, &buf, 1);
    CS_HIGH;

    wpen = ((buf & WPEN_MASK) != 0) ? true : false;
    bp1 = ((buf & BP1_MASK) != 0) ? true : false;
    bp0 = ((buf & BP0_MASK) != 0) ? true : false;
    wel = ((buf & WEL_MASK) != 0) ? true : false;

    // printf("Device %d status register: %x\n", device_id, buf);

    return 0;
}

int MB85RS1MT::write_status_register(uint8_t reg) {
    // WP_HIGH;
    CS_LOW;
    spi_write_blocking(spi, &wrsr, 1);
    spi_write_blocking(spi, &reg, 1);
    CS_HIGH;
    // WP_LOW;

    return 0;
}

int MB85RS1MT::read_memory(uint32_t addr, uint8_t *buf, uint len) {
    uint32_t mod_addr = addr;  // REV24(addr)
    uint8_t addr_buf[] = {(0x00FF0000 & addr) >> 16, (0x0000FF00 & addr) >> 8,
                          0x000000FF & addr};

    CS_LOW;
    spi_write_blocking(spi, &read, 1);
    spi_write_blocking(spi, addr_buf, 3);
    spi_read_blocking(spi, 0, buf, len);
    CS_HIGH;

    return 0;
}

int MB85RS1MT::write_memory(uint32_t addr, uint8_t *buf, uint len) {
    set_wel();

    read_status_register();

    if (!wel) {
        printf("memory not writeable\n");
        return -1;
    }

    if (addr > MAX_ADDR) {
        printf("address too large");
        return -1;
    }

    if (addr + len > MAX_ADDR) {
        len = MAX_ADDR - addr;
    }

    uint32_t mod_addr = addr;  // REV24(addr)
    uint8_t addr_buf[] = {(0x00FF0000 & addr) >> 16, (0x0000FF00 & addr) >> 8,
                          0x000000FF & addr};

    CS_LOW;
    if (spi_write_blocking(spi, &write, 1) != 1) {
        printf("Error writing WRITE op-code");
        return -1;
    }

    if (spi_write_blocking(spi, addr_buf, 3) != 3) {
        printf("Error writing memory address");
        return -1;
    }
    if (spi_write_blocking(spi, buf, len) != len) {
        printf("Error writing data");
        return -1;
    }
    CS_HIGH;

    return 0;
}

int MB85RS1MT::read_device_id() {
    CS_LOW;
    spi_write_blocking(spi, &rdid, 1);
    spi_read_blocking(spi, 0, (uint8_t *)(&device_id), 4);
    CS_HIGH;

    return 0;
}

int MB85RS1MT::mem_init() {
    gpio_set_function(sck_pin, GPIO_FUNC_SPI);
    gpio_set_function(mosi_pin, GPIO_FUNC_SPI);
    gpio_set_function(miso_pin, GPIO_FUNC_SPI);

    gpio_init(cs_pin);
    gpio_set_dir(cs_pin, GPIO_OUT);
    gpio_put(cs_pin, 1);

    if (wp_pin <= 29) {
        gpio_init(wp_pin);
        gpio_set_dir(wp_pin, GPIO_OUT);
        gpio_put(wp_pin, 0);
    }

    if (hold_pin <= 29) {
        gpio_init(hold_pin);
        gpio_set_dir(hold_pin, GPIO_OUT);
        gpio_put(hold_pin, 1);
    }

    device_id = 0;

    read_device_id();
    reset_wel();
    // read_status_register();

    return 0;
}