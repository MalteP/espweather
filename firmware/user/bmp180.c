// #############################################################################
// #                        ESP8266 WiFi Weather Sensor                        #
// #############################################################################
// # bmp180.c - Functions for BMP180 barometer                                 #
// #############################################################################
// #            Version: 1.0 - Compiler: esp-open-sdk 1.5.2 (Linux)            #
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
#include <i2c/i2c.h>
#include "bmp180.h"


// Initialize sensor (read calibration data)
int ICACHE_FLASH_ATTR bmpInit( struct bmpdata* d )
 {
  i2c_init();
  // Read calibration data
  d->ac1 = bmpReadRegister16(0xAA);
  d->ac2 = bmpReadRegister16(0xAC);
  d->ac3 = bmpReadRegister16(0xAE);
  d->ac4 = (uint16_t) bmpReadRegister16(0xB0);
  d->ac5 = (uint16_t) bmpReadRegister16(0xB2);
  d->ac6 = (uint16_t) bmpReadRegister16(0xB4);
  d->b1 = bmpReadRegister16(0xB6);
  d->b2 = bmpReadRegister16(0xB8);
  d->mb = bmpReadRegister16(0xBA);
  d->mc = bmpReadRegister16(0xBC);
  d->md = bmpReadRegister16(0xBE);
  //os_printf("BMP180 calibration values: AC1=%d, AC2=%d, AC3=%d, AC4=%u, AC5=%u, AC6=%u, B1=%d, B2=%d, MB=%d, MC=%d, MD=%d\n", d->ac1, d->ac2, d->ac3, d->ac4, d->ac5, d->ac6, d->b1, d->b2, d->mb, d->mc, d->md );
  return 0;
 }


// Read sensor data and calculate values
int ICACHE_FLASH_ATTR bmpReadSensor( struct bmpdata* d )
 {
  int32_t x1, x2, x3, b3, b5, b6;
  uint32_t b4, b7;
  // Read uncompensated temperature value
  bmpWriteRegister8(0xF4, 0x2E);
  os_delay_us(5000);
  d->ut = bmpReadRegister16(0xF6);
  // Read uncompensated pressure value
  bmpWriteRegister8(0xF4, 0x34+(BMP180_OSS<<6));
  os_delay_us(2000);
  os_delay_us(3000<<BMP180_OSS);
  d->up = bmpReadRegister24(0xF6) >> (8-BMP180_OSS);
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


// Read 16bit register via i2c
int16_t ICACHE_FLASH_ATTR bmpReadRegister16( uint8_t addr )
 {
  uint16_t data;
  i2c_start();
  // Sensor write address
  i2c_writeByte(BMP180_ADDR_W);
  if (!i2c_check_ack())
   {
    i2c_stop();
    return 0;
   }
  // Register address
  i2c_writeByte(addr);
  if (!i2c_check_ack())
   {
    i2c_stop();
    return 0;
   }
  // Repeated start
  i2c_start();
  // Sensor read address
  i2c_writeByte(BMP180_ADDR_R);
  if (!i2c_check_ack())
   {
    i2c_stop();
    return 0;
   }
  // Read and return data
  data = i2c_readByte()<<8;
  i2c_send_ack(1);
  data |= i2c_readByte();
  i2c_stop();
  return data;
 }


// Read 24bit register via i2c
int32_t ICACHE_FLASH_ATTR bmpReadRegister24( uint8_t addr )
 {
  uint32_t data;
  i2c_start();
  // Sensor write address
  i2c_writeByte(BMP180_ADDR_W);
  if (!i2c_check_ack())
   {
    i2c_stop();
    return 0;
   }
  // Register address
  i2c_writeByte(addr);
  if (!i2c_check_ack())
   {
    i2c_stop();
    return 0;
   }
  // Repeated start
  i2c_start();
  // Sensor read address
  i2c_writeByte(BMP180_ADDR_R);
  if (!i2c_check_ack())
   {
    i2c_stop();
    return 0;
   }
  // Read and return data
  data = i2c_readByte()<<16;
  i2c_send_ack(1);
  data |= i2c_readByte()<<8;
  i2c_send_ack(1);
  data |= i2c_readByte();
  i2c_stop();
  return data;
 }


// Write 8bit register via i2c
int ICACHE_FLASH_ATTR bmpWriteRegister8( uint8_t addr, uint8_t value )
 {
  i2c_start();
  // Sensor write address
  i2c_writeByte(BMP180_ADDR_W);
  if (!i2c_check_ack())
   {
    i2c_stop();
    return 0;
   }
  // Register address
  i2c_writeByte(addr);
  if (!i2c_check_ack())
   {
    i2c_stop();
    return 0;
   }
  // Write value
  i2c_writeByte(value);
  if (!i2c_check_ack())
   {
    i2c_stop();
    return 0;
   }
  i2c_stop();
  return 0;
 }
