#include "config.h"

#include <stdio.h>
#include <stdlib.h>
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

    wpen = (buf & WPEN_MASK != 0) ? true : false;
    bp1 = (buf & BP1_MASK != 0) ? true : false;
    bp0 = (buf & BP0_MASK != 0) ? true : false;
    wel = (buf & WEL_MASK != 0) ? true : false;

    printf("Device %d status register: %x\n", device_id, buf);

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
    CS_LOW;
    spi_write_blocking(spi, &read, 1);
    spi_write16_blocking(spi, &addr, 1);
    spi_read_blocking(spi, 0, buf, len);
    CS_HIGH;

    return 0;
}

int MB85RS16N::write_memory(uint16_t addr, uint8_t *buf, uint len) {
    set_wel();

    CS_LOW;
    spi_write_blocking(spi, &write, 1);
    spi_write16_blocking(spi, &addr, 1);
    spi_write_blocking(spi, buf, len);
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
    read_status_register();
    set_wel();
    read_status_register();

    return 0;
}

int MB85RS16N::write_config(Config_name cfg_name, Config cfg, uint8_t *buf,
                            uint len) {
    uint16_t addr = cfg.addrs[cfg_name];
    uint cfg_len = cfg.len[cfg_name];

    printf("Writing %d bytes to address %x\n", cfg_len, addr);

    if (len > cfg_len) {
        printf("config value (%d bytes) too long (%d bytes)\n", len, cfg_len);
        return -1;
    }

    set_wel();

    CS_LOW;
    spi_write_blocking(spi, &write, 1);
    spi_write16_blocking(spi, &addr, 1);
    spi_write_blocking(spi, buf, cfg_len);
    CS_HIGH;

    reset_wel();

    return 0;
}
int MB85RS16N::read_config(Config_name cfg_name, Config cfg, uint8_t *buf,
                           uint len) {
    uint16_t addr = cfg.addrs[cfg_name];
    uint cfg_len = cfg.len[cfg_name];

    printf("Reading %d bytes from address %x\n", cfg_len, addr);

    if (len < cfg_len) {
        printf(
            "give buffer too small (%d bytes) to contain config (%d bytes)\n",
            len, cfg_len);
        return -1;
    }

    CS_LOW;
    printf("Writing op code %d\n", spi_write_blocking(spi, &read, 1));
    printf("Writing address %d\n", spi_write16_blocking(spi, &addr, 1));
    printf("Reading value %d\n", spi_read_blocking(spi, 0, buf, cfg_len));
    CS_HIGH;

    return 0;
}