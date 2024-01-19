#include "MB85RS16N.h"

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

#define REV(x) ((0x00FF & x) << 8) | ((0xFF00 & x) >> 8)  // for 16-bit numbers

int MB85RS16N::set_wel(void) {
    CS_LOW;
    spi_write_blocking(spi, &wren, 1);
    CS_HIGH;

    return 0;
}

int MB85RS16N::reset_wel() {
    CS_LOW;
    spi_write_blocking(spi, &wrdi, 1);
    CS_HIGH;

    return 0;
}

int MB85RS16N::read_status_register() {
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

int MB85RS16N::write_status_register(uint8_t reg) {
    WP_HIGH;
    CS_LOW;
    spi_write_blocking(spi, &wrsr, 1);
    spi_write_blocking(spi, &reg, 1);
    CS_HIGH;
    WP_LOW;

    return 0;
}

int MB85RS16N::read_memory(uint16_t addr, uint8_t *buf, uint len) {
    uint16_t mod_addr = REV(addr);

    CS_LOW;
    spi_write_blocking(spi, &read, 1);
    spi_write_blocking(spi, (uint8_t *)(&mod_addr), 2);
    spi_read_blocking(spi, 0, buf, len);
    CS_HIGH;

    return 0;
}

int MB85RS16N::write_memory(uint16_t addr, uint8_t *buf, uint len) {
    set_wel();

    read_status_register();

    if (!wel) {
        printf("memory not writeable\n");
        return -1;
    }

    uint16_t mod_addr = REV(addr);

    CS_LOW;
    if (spi_write_blocking(spi, &write, 1) != 1) {
        printf("Error writing WRITE op-code");
        return -1;
    }

    if (spi_write_blocking(spi, (uint8_t *)(&mod_addr), 2) != 2) {
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

int MB85RS16N::read_device_id() {
    CS_LOW;
    spi_write_blocking(spi, &rdid, 1);
    spi_read_blocking(spi, 0, (uint8_t *)(&device_id), 4);
    CS_HIGH;

    return 0;
}

int MB85RS16N::mem_init() {
    gpio_set_function(sck_pin, GPIO_FUNC_SPI);
    gpio_set_function(mosi_pin, GPIO_FUNC_SPI);
    gpio_set_function(miso_pin, GPIO_FUNC_SPI);

    gpio_init(cs_pin);
    gpio_set_dir(cs_pin, GPIO_OUT);
    gpio_put(cs_pin, 1);

    gpio_init(wp_pin);
    gpio_set_dir(wp_pin, GPIO_OUT);
    gpio_put(wp_pin, 0);

    gpio_init(hold_pin);
    gpio_set_dir(hold_pin, GPIO_OUT);
    gpio_put(hold_pin, 1);

    read_device_id();
    reset_wel();
    // read_status_register();

    return 0;
}