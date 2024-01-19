#include <stdio.h>
#include <string.h>

#include "../../MB85RS16N.h"
#include "../../config.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "pico/stdlib.h"

spi_inst_t *spi = spi0;
const uint sck_pin = 18;
const uint mosi_pin = 19;
const uint miso_pin = 20;
const uint cs_pin = 1;
const uint wp_pin = 9;
const uint hold_pin = 7;

MB85RS16N mem(spi, cs_pin, sck_pin, mosi_pin, miso_pin, wp_pin, hold_pin);

Config test_config;

int main() {
    stdio_init_all();

    sleep_ms(5000);

    // SPI Settings
    spi_init(spi, 1000 * 1000);

    spi_set_format(spi,            // SPI instance
                   8,              // Number of bits per transfer
                   (spi_cpol_t)0,  // Polarity (CPOL)
                   (spi_cpha_t)0,  // Phase (CPHA)
                   SPI_MSB_FIRST);

    // Init memory device, in this case some FRAM from Adafruit
    mem.mem_init();

    printf("Config Lib Test Compiled %s %s\n", __DATE__, __TIME__);

    printf("Device ID: %d\n", mem.device_id);

    // Read and print the NAME config setting
    read_config(NAME, test_config, (uint8_t *)test_config.name,
                sizeof(test_config.name), &mem);

    printf("DEVICE NAME: %s\n", test_config.name);

    // Write the time that this file was last compiled to the config struct
    strcpy(test_config.name, __TIME__);

    // Write the changed setting to the FRAM
    write_config(NAME, test_config, (uint8_t *)test_config.name,
                 sizeof(test_config.name), &mem);

    // Read and print again
    strcpy(test_config.name, "something broke");

    read_config(NAME, test_config, (uint8_t *)test_config.name,
                sizeof(test_config.name), &mem);

    printf("DEVICE NAME: %s\n", test_config.name);

    while (true) {
    }
}
