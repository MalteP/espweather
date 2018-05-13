// #############################################################################
// #                        ESP8266 WiFi Weather Sensor                        #
// #############################################################################
// # bmp180.c - Functions for BMP180 barometer                                 #
// #############################################################################
// #            Version: 1.2 - Compiler: esp-open-sdk 1.5.2 (Linux)            #
// #  (c) 2015-2016 by Malte Pöggel - www.MALTEPOEGGEL.de - malte@poeggel.de   #
// #############################################################################
// #  This program is free software; you can redistribute it and/or modify it  #
// #   under the terms of the GNU General Public License as published by the   #
// #        Free Software Foundation; either version 3 of the License,         #
// #                  or (at your option) any later version.                   #
// #                                                                           #
// #      This program is distributed in the hope that it will be useful,      #
// #      but WITHOUT ANY WARRANTY; without even the implied warranty of       #
// #           MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.            #
// #           See the GNU General Public License for more details.            #
// #                                                                           #
// #  You should have received a copy of the GNU General Public License along  #
// #      with this program; if not, see <http://www.gnu.org/licenses/>.       #
// #############################################################################

#include <esp8266.h>
#include "i2c-common.h"
#include "bmp180.h"


// Initialize sensor (read calibration data)
int ICACHE_FLASH_ATTR bmpInit( struct bmpdata* d )
 {
  i2cInit();
  if(i2cWriteRegister8(BMP180_ADDR, BMP180_SRESET, BMP180_SRESET_V)!=0) return -1;
  if(i2cReadRegister8(BMP180_ADDR, BMP180_CHIPID)!=BMP180_CHIPID_V) return -1;
  // Read calibration data
  d->ac1 = i2cReadRegister16(BMP180_ADDR, BMP180_REG_AC1);
  d->ac2 = i2cReadRegister16(BMP180_ADDR, BMP180_REG_AC2);
  d->ac3 = i2cReadRegister16(BMP180_ADDR, BMP180_REG_AC3);
  d->ac4 = (uint16_t) i2cReadRegister16(BMP180_ADDR, BMP180_REG_AC4);
  d->ac5 = (uint16_t) i2cReadRegister16(BMP180_ADDR, BMP180_REG_AC5);
  d->ac6 = (uint16_t) i2cReadRegister16(BMP180_ADDR, BMP180_REG_AC6);
  d->b1 = i2cReadRegister16(BMP180_ADDR, BMP180_REG_B1);
  d->b2 = i2cReadRegister16(BMP180_ADDR, BMP180_REG_B2);
  d->mb = i2cReadRegister16(BMP180_ADDR, BMP180_REG_MB);
  d->mc = i2cReadRegister16(BMP180_ADDR, BMP180_REG_MC);
  d->md = i2cReadRegister16(BMP180_ADDR, BMP180_REG_MD);
  //os_printf("BMP180 calibration values: AC1=%d, AC2=%d, AC3=%d, AC4=%u, AC5=%u, AC6=%u, B1=%d, B2=%d, MB=%d, MC=%d, MD=%d\n", d->ac1, d->ac2, d->ac3, d->ac4, d->ac5, d->ac6, d->b1, d->b2, d->mb, d->mc, d->md );
  return 0;
 }


// Read sensor data and calculate values
int ICACHE_FLASH_ATTR bmpReadSensor( struct bmpdata* d )
 {
  int32_t x1, x2, x3, b3, b5, b6;
  uint32_t b4, b7;
  // Read uncompensated temperature value
  if(i2cWriteRegister8(BMP180_ADDR, BMP180_CTRL, BMP180_TEMP)!=0) return -1;
  os_delay_us(5000);
  d->ut = i2cReadRegister16(BMP180_ADDR, BMP180_OUT_MSB);
  // Read uncompensated pressure value
  if(i2cWriteRegister8(BMP180_ADDR, BMP180_CTRL, 0x34+(BMP180_OSS<<6))!=0) return -1;
  os_delay_us(2000);
  os_delay_us(3000<<BMP180_OSS);
  d->up = i2cReadRegister24(BMP180_ADDR, BMP180_OUT_MSB) >> (8-BMP180_OSS);
  // Calculate temperature
  x1 = (d->ut - d->ac6) * d->ac5 / 32768;
  x2 = d->mc * 2048 / (x1 + d->md);
  b5 = x1 + x2;
  d->t = (b5 + 8) / 16;
  // Calculate pressure
  b6 = b5 - 4000;
  x1 = (d->b2 * (b6 * b6 / 4096)) / 2048;
  x2 = d->ac2 * b6 / 2048;
  x3 = x1 + x2;
  b3 = (((d->ac1 * 4 + x3) << BMP180_OSS) + 2) / 4;
  x1 = d->ac3 * b6 / 8192;
  x2 = d->b1 * (b6 * b6 / 4096) / 65536;
  x3 = ((x1 + x2) + 2) / 4;
  b4 = d->ac4 * (uint32_t)(x3 + 32768) / 32768;
  b7 = ((uint32_t)d->up - b3) * (50000 >> BMP180_OSS);
  if(b7 < 0x80000000)
   {
    d->p = (b7 * 2) / b4;
   } else {
    d->p = (b7 / b4) * 2;
   }
  x1 = (d->p / 256) * (d->p / 256);
  x1 = (x1 * 3038) / 65536;
  x2 = (-7357 * d->p) / 65536;
  d->p = d->p + (x1 + x2 + 3791) / 16;
  // Done.
  os_printf("BMP180: t=%ld, p=%ld\n", (long)d->t, (long)d->p);
  return 0;
 }
