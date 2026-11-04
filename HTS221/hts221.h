#ifndef HTS221_H
#define HTS221_H

#include <stdint.h>

#define HTS221_ADDR             0xBE

/* register addresses */
#define HTS221_WHO_AM_I         0x0F
#define HTS221_AV_CONF          0x10
#define HTS221_CTRL_REG1        0x20
#define HTS221_CTRL_REG2        0x21
#define HTS221_STATUS_REG       0x27
#define HTS221_HUMIDITY_OUT_L   0x28
#define HTS221_TEMP_OUT_L       0x2A
#define HTS221_CALIB_START      0x30

/* register values */
#define HTS221_WHO_AM_I_VAL     0xBC
#define HTS221_CTRL1_PD         0x80
#define HTS221_CTRL1_BDU        0x04
#define HTS221_CTRL1_ODR_1HZ    0x01

#define HTS221_AUTO_INC         0x80    /* bit 7 of the register address */
#define HTS221_CALIB_LEN        16

typedef uint8_t (*hts221_io_t)(uint8_t dev_addr, uint8_t reg, uint8_t *data, uint8_t len);

void hts221_set_io(hts221_io_t read, hts221_io_t write);
uint8_t hts221_init(void);
uint8_t hts221_read_reg(uint8_t reg, uint8_t *data, uint8_t len);
uint8_t hts221_write_reg(uint8_t reg, uint8_t *data, uint8_t len);
int16_t hts221_read_temperature_raw(void);
int16_t hts221_read_humidity_raw(void);
int16_t hts221_calc_temperature(int16_t raw);   /* 0.1 degC */
uint8_t hts221_calc_humidity(int16_t raw);      /* %rH */

#endif /* HTS221_H */
