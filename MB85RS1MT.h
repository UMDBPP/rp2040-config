#ifndef MB85RS1MT_H
#define MB85RS1MT_H

#include "hardware/gpio.h"
#include "hardware/spi.h"

// Status Register Bit Masks
// WPEN RFU RFU RFU BP1 BP0 WEL 0
#define WPEN_MASK 0x80
#define BP1_MASK 0x08
#define BP0_MASK 0x04
#define WEL_MASK 0x02
#define MAX_ADDR 131072

class MB85RS1MT {
   public:
    spi_inst_t *spi = spi0;
    uint cs_pin;
    uint sck_pin;
    uint mosi_pin;
    uint miso_pin;
    uint wp_pin;    // write protect pin for status register, protected when
                    // low
    uint hold_pin;  // hold pin for serial transfer, holds when low
    bool wpen = false;
    bool bp1 = false;
    bool bp0 = false;
    bool wel = false;
    uint32_t device_id;

    /**
     * Sets parameters for use with MB85RS16N FRAM memory device.
     */
    MB85RS1MT(spi_inst_t *spi_p, const uint cs, const uint sck, const uint mosi,
              const uint miso, const uint wp, const uint hold) {
        spi = spi_p;
        cs_pin = cs;
        sck_pin = sck;
        mosi_pin = mosi;
        miso_pin = miso;
        wp_pin = wp;
        hold_pin = hold;
        device_id = 0;
    }

    MB85RS1MT(spi_inst_t *spi_p, const uint cs, const uint sck, const uint mosi,
              const uint miso) {
        spi = spi_p;
        cs_pin = cs;
        sck_pin = sck;
        mosi_pin = mosi;
        miso_pin = miso;
        wp_pin = 255;
        hold_pin = 255;
        device_id = 0;
    }
    /**
     * Initializes GPIO and SPI pins but SPI peripheral initialization and
     * settings will need to be configured prior.
     */
    int mem_init();

    /**
     * Reads the status register and fills the object's corresponding fields
     */
    int read_status_register(void);

    /**
     * Reads a given number of bytes from the memory starting at the given
     * address
     * @param addr 232-bit address, only the lower 24 bits are used
     */
    int read_memory(uint32_t addr, uint8_t *buf, uint len);

    /**
     * Writes a given number of bytes to the memory starting at the given
     * address
     * @param addr 32-bit address, only the lower 24 bits are used
     */
    int write_memory(uint32_t addr, uint8_t *buf, uint len);

   private:
    /**
     * Sets the write enable latch to enable writing, must be used before WRSR
     * and WRITE
     */
    int set_wel(void);

    /**
     * Resets the write enable latch to disable writing
     */
    int reset_wel(void);

    /**
     * Writes the given status register to the device
     */
    int write_status_register(uint8_t reg);

    /**
     * Reads the 32-bit device ID and fills the corresponding field
     */
    int read_device_id(void);
};

#endif