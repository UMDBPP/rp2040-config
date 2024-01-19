#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "pico/stdlib.h"

int write_config(Config_name cfg_name, Config cfg, uint8_t *buf, uint len,
                 void (*write_memory)(uint16_t addr, uint8_t *buf, uint len)) {
    uint16_t addr = cfg.addrs[cfg_name];
    uint cfg_len = cfg.len[cfg_name];

    printf("Writing %d bytes to address %x\n", cfg_len, addr);

    if (len > cfg_len) {
        printf("config value (%d bytes) too long (%d bytes)\n", len, cfg_len);
        return -1;
    }

    write_memory(addr, buf, len);

    return 0;
}

int read_config(Config_name cfg_name, Config cfg, uint8_t *buf, uint len,
                void (*read_memory)(uint16_t addr, uint8_t *buf, uint len)) {
    uint16_t addr = cfg.addrs[cfg_name];
    uint cfg_len = cfg.len[cfg_name];

    printf("Reading %d bytes from address %x\n", cfg_len, addr);

    if (len < cfg_len) {
        printf(
            "give buffer too small (%d bytes) to contain config (%d bytes)\n",
            len, cfg_len);
        return -1;
    }

    read_memory(addr, buf, len);

    return 0;
}
