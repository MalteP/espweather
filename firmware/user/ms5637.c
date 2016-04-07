// #############################################################################
// #                        ESP8266 WiFi Weather Sensor                        #
// #############################################################################
// # ms5637.c - Functions for MS5637 barometer                                 #
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
#include "i2c-common.h"
#include "ms5637.h"


// Initialize sensor (read calibration data)
int ICACHE_FLASH_ATTR msInit( struct msdata* d )
 {
  i2c_init();
  if(i2cWriteCmd(MS5637_ADDR, MS5637_RESET, I2C_SEND_STOP)!=0) return -1; // Reset sensor
  // Read calibration data
  #ifdef MS5637_ENABLE_CRC_CHECK
  d->c[0] = (uint16_t) i2cReadRegister16(MS5637_ADDR, MS5637_PROM_READ|0x00, I2C_SEND_STOP);
  #else
  d->c[0] = 0;
  #endif
  d->c[1] = (uint16_t) i2cReadRegister16(MS5637_ADDR, MS5637_PROM_READ|0x02, I2C_SEND_STOP);
  d->c[2] = (uint16_t) i2cReadRegister16(MS5637_ADDR, MS5637_PROM_READ|0x04, I2C_SEND_STOP);
  d->c[3] = (uint16_t) i2cReadRegister16(MS5637_ADDR, MS5637_PROM_READ|0x06, I2C_SEND_STOP);
  d->c[4] = (uint16_t) i2cReadRegister16(MS5637_ADDR, MS5637_PROM_READ|0x08, I2C_SEND_STOP);
  d->c[5] = (uint16_t) i2cReadRegister16(MS5637_ADDR, MS5637_PROM_READ|0x0A, I2C_SEND_STOP);
  d->c[6] = (uint16_t) i2cReadRegister16(MS5637_ADDR, MS5637_PROM_READ|0x0C, I2C_SEND_STOP);
  d->c[7] = 0;
  #ifdef MS5637_ENABLE_CRC_CHECK
  if(msCheckCRC(&d->c[0])!=((d->c[0]>>12)&0x000F))
   {
    os_printf("MS5637: checksum mismatch\n");
    return -1;
   }
  #endif
  //os_printf("MS5637 calibration values: C1=%u, C2=%u, C3=%u, C4=%u, C5=%u, C6=%u\n", d->c[1], d->c[2], d->c[3], d->c[4], d->c[5], d->c[6] );
  return 0;
 }


// Read sensor data and calculate values
int ICACHE_FLASH_ATTR msReadSensor( struct msdata* d )
 {
  int32_t dt;
  int64_t off, sens;
  #ifdef MS5637_ENABLE_COMPENSATION
  int64_t t2, off2, sens2;
  #endif
  // Read D1: uncompensated digital pressure value
  if(i2cWriteCmd(MS5637_ADDR, MS5637_CONVERT_D1|MS5637_OSR_PRES, I2C_SEND_STOP)!=0) return -1;
  os_delay_us(540);
  os_delay_us(540<<MS5637_OS_PRES);
  d->d1 = (uint32_t) i2cReadRegister24(MS5637_ADDR, MS5637_ADC_READ, I2C_SEND_STOP);
  // Read D2: digital temperature value
  if(i2cWriteCmd(MS5637_ADDR, MS5637_CONVERT_D2|MS5637_OSR_TEMP, I2C_SEND_STOP)!=0) return -1;
  os_delay_us(540);
  os_delay_us(540<<MS5637_OS_TEMP);
  d->d2 = (uint32_t) i2cReadRegister24(MS5637_ADDR, MS5637_ADC_READ, I2C_SEND_STOP);
  //os_printf("MS5637 read: D1=%u, D2=%u\n", d->d1, d->d2);
  // Calculate temperature
  dt = d->d2 - (int32_t) d->c[5] * (1L<<8);
  d->t = 2000 + (int64_t) dt * (int64_t) d->c[6] / (1LL<<23);
  #ifdef MS5637_ENABLE_COMPENSATION
  if(d->t<2000)
   {
    t2 = 3 * ((int64_t) dt * (int64_t) dt) / (1LL<<33); // Low temperature
   } else {
    t2 = 5 * ((int64_t) dt * (int64_t) dt) / (1LL<<38); // High temperature
   }
  d->t -= t2;
  #endif
  // Calculate pressure
  off = (int64_t) d->c[2] * (1LL<<17) + (int64_t) dt * (int64_t) d->c[4] / (1LL<<6);
  sens = (int64_t) d->c[1] * (1LL<<16) + (int64_t) dt * (int64_t) d->c[3] / (1LL<<7);
  #ifdef MS5637_ENABLE_COMPENSATION
  if(d->t<2000)
   {
    // Low temperature
    off2 = 61 * (int64_t) (d->t-2000) * (int64_t) (d->t-2000) / (1LL<<4);
    sens2 = 29 * (int64_t) (d->t-2000) * (int64_t) (d->t-2000) / (1LL<<4);
    if(d->t<-1500)
     {
      // Very low temperature
      off2 += 17 * (int64_t) (d->t+1500) * (int64_t) (d->t+1500);
      sens2 += 9 * (int64_t) (d->t+1500) * (int64_t) (d->t+1500);
     }
    off -= off2;
    sens -= sens2;
   }
  #endif
  d->p = ((uint64_t) d->d1 * sens / (1LL<<21) - off) / (1LL << 15);
  os_printf("MS5637: t=%d, p=%d\n", d->t, d->p);
  return 0;
 }


#ifdef MS5637_ENABLE_CRC_CHECK
uint16_t ICACHE_FLASH_ATTR msCheckCRC( uint16_t eep[] )
 {
  uint16_t cnt, bit, crc, crc_orig;
  crc = 0;
  crc_orig = eep[0];
  eep[0] = eep[0] & 0x0FFF;
  eep[7] = 0;
  for(cnt=0; cnt<16; cnt++)
   {
    if(cnt%2==1)
     {
      crc ^= (eep[cnt>>1]) & 0x00FF;
     } else {
      crc ^= eep[cnt>>1]>>8;
     }
    for(bit=8; bit>0; bit--)
     {
      if(crc&(0x8000))
       {
        crc = (crc << 1) ^ 0x3000;
       } else {
        crc = (crc << 1);
       }
     }
   }
  eep[0] = crc_orig;
  crc = (crc>>12) & 0x000F;
  return (crc ^ 0x00);
 }
#endif
