#ifndef LPS25HB_H
#define LPS25HB_H

#include <stdint.h>

#define LPS25HB_ADDR            0xBA

/* register addresses */
#define LPS25HB_WHO_AM_I        0x0F
#define LPS25HB_RES_CONF        0x10
#define LPS25HB_CTRL_REG1       0x20
#define LPS25HB_CTRL_REG2       0x21
#define LPS25HB_STATUS_REG      0x27
#define LPS25HB_PRESS_OUT_XL    0x28
#define LPS25HB_TEMP_OUT_L      0x2B

/* register values */
#define LPS25HB_WHO_AM_I_VAL    0xBD
#define LPS25HB_CTRL1_PD        0x80
#define LPS25HB_CTRL1_ODR_1HZ   0x10
#define LPS25HB_CTRL1_BDU       0x04
#define LPS25HB_AVGP_32         0x01    /* 32 vzoriek tlaku */
#define LPS25HB_AVGT_16         0x04    /* 16 vzoriek teploty */

#define LPS25HB_AUTO_INC        0x80    /* bit 7 of the register address */

typedef uint8_t (*lps25hb_io_t)(uint8_t dev_addr, uint8_t reg, uint8_t *data, uint8_t len);

void lps25hb_set_io(lps25hb_io_t read, lps25hb_io_t write);
uint8_t lps25hb_init(void);
uint8_t lps25hb_read_reg(uint8_t reg, uint8_t *data, uint8_t len);
uint8_t lps25hb_write_reg(uint8_t reg, uint8_t *data, uint8_t len);
int32_t lps25hb_get_pressure(void);                 /* 0.01 hPa */
void lps25hb_set_ref_pressure(int32_t pressure);
int32_t lps25hb_get_altitude(int32_t pressure);     /* 0.01 m */

#endif /* LPS25HB_H */
