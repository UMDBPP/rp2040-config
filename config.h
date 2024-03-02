#ifndef CONFIG_H
#define CONFIG_H

#include "MB85RS1MT.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"

#define NUM_CONFIGS 2

#define NAME_ADDR 1
#define NAME_LENGTH 20

#define ID_ADDR (NAME_ADDR + NAME_LENGTH)
#define ID_LENGTH 1

typedef enum Config_Names { NAME = 0, ID } Config_name;

typedef struct Config {
    uint16_t addrs[NUM_CONFIGS] = {NAME_ADDR, ID_ADDR};
    uint len[NUM_CONFIGS] = {NAME_LENGTH, ID_LENGTH};

    char name[NAME_LENGTH] = {0};
    uint8_t id = 0;
} Config;

int write_config(Config_name cfg_name, Config cfg, uint8_t *buf, uint len,
                 MB85RS1MT *mem);
int read_config(Config_name cfg_name, Config cfg, uint8_t *buf, uint len,
                MB85RS1MT *mem);

#endif