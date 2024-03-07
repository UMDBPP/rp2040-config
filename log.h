#ifndef LOG_H
#define LOG_H

#define INIT_ADDR 8192

void write_log(uint8_t *buf, uint len);
void dump_log(void);

#endif