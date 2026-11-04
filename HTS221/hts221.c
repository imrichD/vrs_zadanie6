#include "hts221.h"

static hts221_io_t io_read;
static hts221_io_t io_write;

/* calibration values read once at init */
static uint8_t h0_rh_x2;
static uint8_t h1_rh_x2;
static int16_t t0_deg_x8;
static int16_t t1_deg_x8;
static int16_t h0_t0_out;
static int16_t h1_t0_out;
static int16_t t0_out;
static int16_t t1_out;

void hts221_set_io(hts221_io_t read, hts221_io_t write)
{
  io_read = read;
  io_write = write;
}

uint8_t hts221_read_reg(uint8_t reg, uint8_t *data, uint8_t len)
{
  if (len > 1)
  {
    reg |= HTS221_AUTO_INC;
  }
  return io_read(HTS221_ADDR, reg, data, len);
}

uint8_t hts221_write_reg(uint8_t reg, uint8_t *data, uint8_t len)
{
  if (len > 1)
  {
    reg |= HTS221_AUTO_INC;
  }
  return io_write(HTS221_ADDR, reg, data, len);
}

/* all calibration registers in one transfer */
static uint8_t read_calib(void)
{
  uint8_t c[HTS221_CALIB_LEN];

  if (!hts221_read_reg(HTS221_CALIB_START, c, HTS221_CALIB_LEN))
  {
    return 0;
  }

  h0_rh_x2  = c[0];
  h1_rh_x2  = c[1];
  /* 2 msb bits of both temperatures are in register 0x35 */
  t0_deg_x8 = (int16_t)(c[2] | ((c[5] & 0x03) << 8));
  t1_deg_x8 = (int16_t)(c[3] | ((c[5] & 0x0C) << 6));
  h0_t0_out = (int16_t)(c[6]  | (c[7]  << 8));
  h1_t0_out = (int16_t)(c[10] | (c[11] << 8));
  t0_out    = (int16_t)(c[12] | (c[13] << 8));
  t1_out    = (int16_t)(c[14] | (c[15] << 8));
  return 1;
}

uint8_t hts221_init(void)
{
  uint8_t id;
  uint8_t av = HTS221_AVGT_16 | HTS221_AVGH_32;
  uint8_t cfg = HTS221_CTRL1_PD | HTS221_CTRL1_BDU | HTS221_CTRL1_ODR_1HZ;

  if (!hts221_read_reg(HTS221_WHO_AM_I, &id, 1))
  {
    return 0;
  }
  if (id != HTS221_WHO_AM_I_VAL)
  {
    return 0;
  }
  /* priemerovanie sa nastavuje pred zapnutim senzora */
  if (!hts221_write_reg(HTS221_AV_CONF, &av, 1))
  {
    return 0;
  }
  if (!hts221_write_reg(HTS221_CTRL_REG1, &cfg, 1))
  {
    return 0;
  }
  return read_calib();
}

int16_t hts221_read_temperature_raw(void)
{
  uint8_t raw[2];

  hts221_read_reg(HTS221_TEMP_OUT_L, raw, 2);
  return (int16_t)(raw[0] | (raw[1] << 8));
}

int16_t hts221_read_humidity_raw(void)
{
  uint8_t raw[2];

  hts221_read_reg(HTS221_HUMIDITY_OUT_L, raw, 2);
  return (int16_t)(raw[0] | (raw[1] << 8));
}

/* linear interpolation between the two calibration points */
int16_t hts221_calc_temperature(int16_t raw)
{
  int32_t t_x8;

  t_x8 = t0_deg_x8 + ((int32_t)(raw - t0_out) * (t1_deg_x8 - t0_deg_x8)) / (t1_out - t0_out);
  return (int16_t)(t_x8 * 10 / 8);
}

uint8_t hts221_calc_humidity(int16_t raw)
{
  int32_t h_x2;

  h_x2 = h0_rh_x2 + ((int32_t)(raw - h0_t0_out) * (h1_rh_x2 - h0_rh_x2)) / (h1_t0_out - h0_t0_out);
  return (uint8_t)(h_x2 / 2);
}
