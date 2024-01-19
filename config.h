#ifndef CONFIG_H
#define CONFIG_H

#include "MB85RS16N.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"

#define NUM_CONFIGS 1
#define NAME_ADDR 1
#define NAME_LENGTH 20

typedef enum Config_Names { NAME = 0 } Config_name;

typedef struct Config {
    uint16_t addrs[NUM_CONFIGS] = {NAME_ADDR};
    uint len[NUM_CONFIGS] = {NAME_LENGTH};
    char name[NAME_LENGTH] = {0};
} Config;

int write_config(Config_name cfg_name, Config cfg, uint8_t *buf, uint len,
                 MB85RS16N *mem);
int read_config(Config_name cfg_name, Config cfg, uint8_t *buf, uint len,
                MB85RS16N *mem);

#endif