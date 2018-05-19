// #############################################################################
// #                        ESP8266 WiFi Weather Sensor                        #
// #############################################################################
// # bmx280.c - Functions for BMP280 and BME280 temp. / barometer / humidity   #
// #############################################################################
// #            Version: 1.2 - Compiler: esp-open-sdk 1.5.2 (Linux)            #
// #  (c) 2015-2018 by Malte Pöggel - www.MALTEPOEGGEL.de - malte@poeggel.de   #
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
#include "bmx280.h"


// Initialize sensor (read calibration data)
int ICACHE_FLASH_ATTR bmx280Init( struct bmx280data* d )
 {
  uint8_t chip_id;
  uint16_t temp_h4;
  i2cInit();
  if(i2cWriteRegister8(BMX280_ADDR, BMX280_SRESET, BMX280_SRESET_V)!=0) return -1;
  chip_id = i2cReadRegister8(BMX280_ADDR, BMX280_CHIPID);
  if(chip_id==BMX280_ID_BMP1||chip_id==BMX280_ID_BMP2||chip_id==BMX280_ID_BMP3)
   {
    d->sensor_type = TYPE_BMP280;
   } else {
    if(chip_id==BMX280_ID_BME)
     {
      d->sensor_type = TYPE_BME280;
     } else {
      return -1;
     }
   }
  // Read calibration data
  d->t1 = i2cReadRegister16LE(BMX280_ADDR, BMX280_REG_T1);
  d->t2 = (int16_t) i2cReadRegister16LE(BMX280_ADDR, BMX280_REG_T2);
  d->t3 = (int16_t) i2cReadRegister16LE(BMX280_ADDR, BMX280_REG_T3);
  d->p1 = i2cReadRegister16LE(BMX280_ADDR, BMX280_REG_P1);
  d->p2 = (int16_t) i2cReadRegister16LE(BMX280_ADDR, BMX280_REG_P2);
  d->p3 = (int16_t) i2cReadRegister16LE(BMX280_ADDR, BMX280_REG_P3);
  d->p4 = (int16_t) i2cReadRegister16LE(BMX280_ADDR, BMX280_REG_P4);
  d->p5 = (int16_t) i2cReadRegister16LE(BMX280_ADDR, BMX280_REG_P5);
  d->p6 = (int16_t) i2cReadRegister16LE(BMX280_ADDR, BMX280_REG_P6);
  d->p7 = (int16_t) i2cReadRegister16LE(BMX280_ADDR, BMX280_REG_P7);
  d->p8 = (int16_t) i2cReadRegister16LE(BMX280_ADDR, BMX280_REG_P8);
  d->p9 = (int16_t) i2cReadRegister16LE(BMX280_ADDR, BMX280_REG_P9);
  if(d->sensor_type==TYPE_BME280)
   {
    d->h1 = i2cReadRegister8(BMX280_ADDR, BMX280_REG_H1);
    d->h2 = (int16_t) i2cReadRegister16LE(BMX280_ADDR, BMX280_REG_H2);
    d->h3 = i2cReadRegister8(BMX280_ADDR, BMX280_REG_H3);
    temp_h4 = i2cReadRegister16(BMX280_ADDR, BMX280_REG_H4);
    d->h4 = (int16_t) ((temp_h4 >> 4 & 0x0FF0) | (temp_h4 & 0x000F));
    d->h5 = (int16_t) (i2cReadRegister16LE(BMX280_ADDR, BMX280_REG_H5) >> 4);
    d->h6 = (int8_t) i2cReadRegister8(BMX280_ADDR, BMX280_REG_H6);
   } else {
    d->h1 = 0;
    d->h2 = 0;
    d->h3 = 0;
    d->h4 = 0;
    d->h5 = 0;
    d->h6 = 0;
   }
  //os_printf("BMX280 calibration values: T1=%u, T2=%d, T3=%d, P1=%u, P2=%d, P3=%d, P4=%d, P5=%d, P6=%d, P7=%d, P8=%d, P9=%d, H1=%u, H2=%d, H3=%u, H4=%d, H5=%d, H6=%d\nBMX280 sensor type: %u\n", d->t1, d->t2, d->t3, d->p1, d->p2, d->p3, d->p4, d->p5, d->p6, d->p7, d->p8, d->p9, d->h1, d->h2, d->h3, d->h4, d->h5, d->h6, d->sensor_type );
  return 0;
 }


// Read sensor data and calculate values
int ICACHE_FLASH_ATTR bmx280Read( struct bmx280data* d )
 {
  uint32_t adc_t, adc_p, adc_h;
  // Force read, Oversampling: osrs_t 1x, osrs_p 1x, osrs_h 1x
  if(d->sensor_type==TYPE_BME280)
   {
    if(i2cWriteRegister8(BMX280_ADDR, BMX280_CTRL_HUM, 0x01)!=0) return -1;
   }
  if(i2cWriteRegister8(BMX280_ADDR, BMX280_CTRL, 0x25)!=0) return -1;
  // Wait until measurement is completed
  os_delay_us(8000);
  // Read uncompensated temperature value
  adc_t = (uint32_t)i2cReadRegister24(BMX280_ADDR, BMX280_REG_TEMP) >> 4;
  // Calculate temperature
  bmx280CompensateTemperature(d, adc_t);
  // Read uncompensated pressure value
  adc_p = (uint32_t)i2cReadRegister24(BMX280_ADDR, BMX280_REG_PRES) >> 4;
  // Calculate pressure
  bmx280CompensatePressure(d, adc_p);
  if(d->sensor_type==TYPE_BME280)
   {
    // Read uncompensated humidity value
    adc_h = (uint32_t)i2cReadRegister16(BMX280_ADDR, BMX280_REG_HUM);
    // Calculate humidity
    bmx280CompensateHumidity(d, adc_h);
   } else {
    d->humidity = 0;
   }
  os_printf("BMX280 t=%d p=%d h=%d\n", d->temperature, d->pressure, d->humidity);
  return 0;
 }


// Calculate compensated temperature (code from datasheet)
int ICACHE_FLASH_ATTR bmx280CompensateTemperature( struct bmx280data* d, uint32_t adc_t )
 {
  int32_t var1, var2;
  var1 = ((((adc_t>>3) - ((int32_t)d->t1<<1))) * ((int32_t)d->t2)) >> 11;
  var2 = (((((adc_t>>4) - ((int32_t)d->t1)) * ((adc_t>>4) - ((int32_t)d->t1))) >> 12) * ((int32_t)d->t3)) >> 14;
  d->t_fine = var1 + var2;
  d->temperature = (d->t_fine * 5 + 128) >> 8;
  return 0;
 }


// Calculate compensated pressure (code from datasheet)
int ICACHE_FLASH_ATTR bmx280CompensatePressure( struct bmx280data* d, uint32_t adc_p )
 {
  int32_t var1, var2;
  var1 = (((int32_t)d->t_fine)>>1) - (int32_t)64000;
  var2 = (((var1>>2) * (var1>>2)) >> 11 ) * ((int32_t)d->p6);
  var2 = var2 + ((var1*((int32_t)d->p5))<<1);
  var2 = (var2>>2) + (((int32_t)d->p4)<<16);
  var1 = (((d->p3 * (((var1>>2) * (var1>>2)) >> 13 )) >> 3) + ((((int32_t)d->p2) * var1)>>1))>>18;
  var1 =((((32768+var1))*((int32_t)d->p1))>>15);
  if(var1 == 0) // Avoid exception caused by division by zero
   {
    d->pressure = 0;
    return -1;
   }
  d->pressure = (((uint32_t)(((int32_t)1048576)-adc_p)-(var2>>12)))*3125;
  if(d->pressure < 0x80000000)
   {
    d->pressure = (d->pressure << 1) / ((uint32_t)var1);
   } else {
    d->pressure = (d->pressure / (uint32_t)var1) * 2;
   }
  var1 = (((int32_t)d->p9) * ((int32_t)(((d->pressure>>3) * (d->pressure>>3))>>13)))>>12;
  var2 = (((int32_t)(d->pressure>>2)) * ((int32_t)d->p8))>>13;
  d->pressure = (uint32_t)((int32_t)d->pressure + ((var1 + var2 + d->p7) >> 4));
  return 0;
 }


// Calculate compensated humidity (code from datasheet)
int ICACHE_FLASH_ATTR bmx280CompensateHumidity( struct bmx280data* d, uint32_t adc_h )
 {
  int32_t var1;
  var1 = (d->t_fine - ((int32_t)76800));
  var1 = (((((adc_h << 14) - (((int32_t)d->h4) << 20) - (((int32_t)d->h5) * var1)) + ((int32_t)16384)) >> 15) * (((((((var1 * ((int32_t)d->h6)) >> 10) * (((var1 * ((int32_t)d->h3)) >> 11) + ((int32_t)32768))) >> 10) + ((int32_t)2097152)) * ((int32_t)d->h2) + 8192) >> 14));
  var1 = (var1 - (((((var1 >> 15) * (var1 >> 15)) >> 7) * ((int32_t)d->h1)) >> 4));
  var1 = (var1 < 0 ? 0 : var1);
  var1 = (var1 > 409600000 ? 409600000 : var1);
  d->humidity = (var1>>12);
  return 0;
 }
