#ifndef LOG_H
#define LOG_H

#define INIT_ADDR 8192

// typedef struct _log_config {} log_config;

void write_log(uint8_t *buf, uint len);
void dump_log(void);
// extern string log_to_string();

#endif