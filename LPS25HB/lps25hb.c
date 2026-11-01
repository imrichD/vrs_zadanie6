#include "lps25hb.h"
#include <math.h>

static lps25hb_io_t io_read;
static lps25hb_io_t io_write;

/* pressure measured at startup, used as the altitude reference */
static float ref_pressure;

void lps25hb_set_io(lps25hb_io_t read, lps25hb_io_t write)
{
  io_read = read;
  io_write = write;
}

uint8_t lps25hb_read_reg(uint8_t reg, uint8_t *data, uint8_t len)
{
  if (len > 1)
  {
    reg |= LPS25HB_AUTO_INC;
  }
  return io_read(LPS25HB_ADDR, reg, data, len);
}

uint8_t lps25hb_write_reg(uint8_t reg, uint8_t *data, uint8_t len)
{
  if (len > 1)
  {
    reg |= LPS25HB_AUTO_INC;
  }
  return io_write(LPS25HB_ADDR, reg, data, len);
}

uint8_t lps25hb_init(void)
{
  uint8_t id;
  uint8_t cfg = LPS25HB_CTRL1_PD | LPS25HB_CTRL1_ODR_1HZ | LPS25HB_CTRL1_BDU;

  if (!lps25hb_read_reg(LPS25HB_WHO_AM_I, &id, 1))
  {
    return 0;
  }
  if (id != LPS25HB_WHO_AM_I_VAL)
  {
    return 0;
  }
  return lps25hb_write_reg(LPS25HB_CTRL_REG1, &cfg, 1);
}

/* all 3 pressure bytes in one transfer */
int32_t lps25hb_get_pressure(void)
{
  uint8_t raw[3];
  int32_t p;

  lps25hb_read_reg(LPS25HB_PRESS_OUT_XL, raw, 3);
  p = ((int32_t)raw[2] << 16) | ((int32_t)raw[1] << 8) | raw[0];
  if (p & 0x800000)       /* 24 bit signed value */
  {
    p -= 0x1000000;
  }
  /* 4096 LSB per hPa */
  return p * 100 / 4096;
}

void lps25hb_set_ref_pressure(int32_t pressure)
{
  ref_pressure = (float)pressure;
}

/* relative altitude against the startup pressure, barometric formula */
int32_t lps25hb_get_altitude(int32_t pressure)
{
  float h;

  h = 44330.0f * (1.0f - powf((float)pressure / ref_pressure, 1.0f / 5.255f));
  return (int32_t)(h * 100.0f);
}
