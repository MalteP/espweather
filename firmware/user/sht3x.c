// #############################################################################
// #                        ESP8266 WiFi Weather Sensor                        #
// #############################################################################
// # sht3x.c - Functions for SHT30/31/35 temperature / humidity sensor         #
// #############################################################################
// #            Version: 1.2 - Compiler: esp-open-sdk 2.1.0 (Linux)            #
// #  (c) 2015-2019 by Malte Pöggel - www.MALTEPOEGGEL.de - malte@poeggel.de   #
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
#include "sht3x.h"


// Initialize sensor
int ICACHE_FLASH_ATTR sht3Init( struct sht3data* d )
 {
  // Reset data
  d->temperature = 0;
  d->humidity = 0;
  // Initialize I2C
  i2cInit();
  // Reset sensor
  if(i2cWriteRegister8(SHT3x_ADDR, SHT3x_SRESET_MSB, SHT3x_SRESET_LSB)!=0) return -1;
  return 0;
 }

// Read temperature & humidity
int ICACHE_FLASH_ATTR sht3Read( struct sht3data* d )
 {
  int sht_wait = 25; // Wait 25ms max for sensor to finish measurement
  uint16_t temperature, humidity;
  #ifdef SHT3_USE_CRC
  uint8_t temperature_crc, humidity_crc;
  #endif
  // Start conversion
  if(i2cWriteRegister8(SHT3x_ADDR, SHT3x_MS_HIGH_MSB, SHT3x_MS_HIGH_LSB)!=0) return -1;
  // Wait until measurement is finished
  while(1)
   {
    i2cSendStart();
    if(i2cWriteByte((SHT3x_ADDR<<1)|1))
     {
      // Got NACK, try again after 1ms delay
      i2cSendStop();
      os_delay_us(1000);
      if(--sht_wait==0)
       {
        return -1;
       }
     } else {
      // Got ACK, measurement completed
      break;
     }
   }
  // Read data
  temperature = i2cReadByte(1);
  temperature <<= 8;
  temperature |= i2cReadByte(1);
  temperature_crc = i2cReadByte(1);
  humidity = i2cReadByte(1);
  humidity <<= 8;
  humidity |= i2cReadByte(1);
  humidity_crc = i2cReadByte(0);
  i2cSendStop();
  #ifdef SHT3_USE_CRC
  // Check CRC
  if(temperature_crc!=sht3CalculateCRC(0xFF, temperature)||humidity_crc!=sht3CalculateCRC(0xFF, humidity))
   {
    os_printf("SHT3x: checksum mismatch\n");
    return -1;
   }
  #endif
  // Calculate compensated temperature and humidity
  // T = -45 + 175 * ST / 2^16 | *100 ->  T = 4375 * ST / 2^14 -4500
  d->temperature = ((4375*(int)temperature)>>14) - 4500;
  // RH = 100 * SRH / 2^16     | *100 -> RH = 625 * SRH / 2^12
  d->humidity = (625*(unsigned int)humidity)>>12;
  os_printf("SHT3x t=%d, h=%d\n", d->temperature, d->humidity);
  return 0;
 }

#ifdef SHT3_USE_CRC
// Check CRC of sensor data
uint8_t ICACHE_FLASH_ATTR sht3CalculateCRC( uint8_t start, uint16_t byte )
 {
  uint8_t crc = start;
  uint8_t i;
  crc ^= (uint8_t)(byte>>8);
  for(i=0; i<8; i++)
   {
    if(crc&0x80)
     {
      crc = (crc << 1)^SHT3_CRC_POLYNOMIAL;
     } else {
      crc <<= 1;
     }
   }
  crc ^= (uint8_t)byte;
  for(i=0; i<8; i++)
   {
    if(crc&0x80)
     {
      crc = (crc << 1)^SHT3_CRC_POLYNOMIAL;
     } else {
      crc <<= 1;
     }
   }
  return crc;
 }
#endif
