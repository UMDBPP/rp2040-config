#include "log.h"

#include <stdio.h>
#include <string.h>

#include "MB85RS1MT.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "pico/stdlib.h"

static uint32_t current_addr = INIT_ADDR;

void write_log(uint8_t *buf, uint len) {
    // write_memory(current_addr, buf, len);
    // current_addr = current_addr + len;
}

void dump_log(void) {
    // read_memory();
}
