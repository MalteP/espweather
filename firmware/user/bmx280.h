// #############################################################################
// #                        ESP8266 WiFi Weather Sensor                        #
// #############################################################################
// # bmx280.h - Functions for BMP280 and BME280 temp. / barometer / humidity   #
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

#ifndef BMX280_H
 #define BMX280_H

 #define BMX280_ADDR     0x77

 #define BMX280_REG_T1   0x88
 #define BMX280_REG_T2   0x8A
 #define BMX280_REG_T3   0x8C
 #define BMX280_REG_P1   0x8E
 #define BMX280_REG_P2   0x90
 #define BMX280_REG_P3   0x92
 #define BMX280_REG_P4   0x94
 #define BMX280_REG_P5   0x96
 #define BMX280_REG_P6   0x98
 #define BMX280_REG_P7   0x9A
 #define BMX280_REG_P8   0x9C
 #define BMX280_REG_P9   0x9E
 #define BMX280_REG_H1   0xA1
 #define BMX280_REG_H2   0xE1
 #define BMX280_REG_H3   0xE3
 #define BMX280_REG_H4   0xE4
 #define BMX280_REG_H5   0xE5
 #define BMX280_REG_H6   0xE7
 #define BMX280_CTRL     0xF4
 #define BMX280_CTRL_HUM 0xF2
 #define BMX280_SRESET   0xE0
 #define BMX280_SRESET_V 0xB6
 #define BMX280_CHIPID   0xD0
 #define BMX280_ID_BMP1  0x56
 #define BMX280_ID_BMP2  0x57
 #define BMX280_ID_BMP3  0x58
 #define BMX280_ID_BME   0x60
 #define BMX280_REG_TEMP 0xFA
 #define BMX280_REG_PRES 0xF7
 #define BMX280_REG_HUM  0xFD

 #define TYPE_BMP280 0
 #define TYPE_BME280 1

 // Structure for sensor data
 struct bmx280data
  {
   uint16_t t1;
   int16_t t2;
   int16_t t3;
   uint16_t p1;
   int16_t p2;
   int16_t p3;
   int16_t p4;
   int16_t p5;
   int16_t p6;
   int16_t p7;
   int16_t p8;
   int16_t p9;
   uint8_t h1;
   int16_t h2;
   uint8_t h3;
   int16_t h4;
   int16_t h5;
   int8_t h6;
   uint8_t sensor_type;
   int32_t t_fine;
   int32_t temperature;
   uint32_t pressure;
   uint32_t humidity;
  };

 // Functions
 int bmx280Init( struct bmx280data* d );
 int bmx280Read( struct bmx280data* d );

 // Internal functions
 int bmx280CompensateTemperature( struct bmx280data* d, uint32_t adc_t );
 int bmx280CompensatePressure( struct bmx280data* d, uint32_t adc_p );
 int bmx280CompensateHumidity( struct bmx280data* d, uint32_t adc_h );

#endif
