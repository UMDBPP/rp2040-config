#include "config.h"

#include <stdio.h>
#include <string.h>

#include "MB85RS1MT.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "pico/stdlib.h"

int write_config(Config_name cfg_name, Config cfg, uint8_t *buf, uint len,
                 MB85RS1MT *mem) {
    uint32_t addr = cfg.addrs[cfg_name];
    uint cfg_len = cfg.len[cfg_name];

    // printf("Writing %d bytes to address %x\n", cfg_len, addr);

    if (len > cfg_len) {
        printf("config value (%d bytes) too long (%d bytes)\n", len, cfg_len);
        return -1;
    }

    mem->write_memory(addr, buf, len);

    return 0;
}

int read_config(Config_name cfg_name, Config cfg, uint8_t *buf, uint len,
                MB85RS1MT *mem) {
    uint32_t addr = cfg.addrs[cfg_name];
    uint cfg_len = cfg.len[cfg_name];

    // printf("Reading %d bytes from address %x\n", cfg_len, addr);

    if (len < cfg_len) {
        printf(
            "given buffer too small (%d bytes) to contain config (%d bytes)\n",
            len, cfg_len);
        return -1;
    }

    mem->read_memory(addr, buf, len);

    return 0;
}
