// #############################################################################
// #                        ESP8266 WiFi Weather Sensor                        #
// #############################################################################
// # bmp180.h - Functions for BMP180 barometer                                 #
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

#ifndef BMP180_H
 #define BMP180_H

 #define BMP180_ADDR     0x77

 #define BMP180_REG_AC1  0xAA
 #define BMP180_REG_AC2  0xAC
 #define BMP180_REG_AC3  0xAE
 #define BMP180_REG_AC4  0xB0
 #define BMP180_REG_AC5  0xB2
 #define BMP180_REG_AC6  0xB4
 #define BMP180_REG_B1   0xB6
 #define BMP180_REG_B2   0xB8
 #define BMP180_REG_MB   0xBA
 #define BMP180_REG_MC   0xBC
 #define BMP180_REG_MD   0xBE
 #define BMP180_CTRL     0xF4
 #define BMP180_SRESET   0xE0
 #define BMP180_SRESET_V 0xB6
 #define BMP180_CHIPID   0xD0
 #define BMP180_CHIPID_V 0x55
 #define BMP180_TEMP     0x2E
 #define BMP180_OUT_MSB  0xF6
 #define BMP180_OUT_LSB  0xF7
 #define BMP180_OUT_XLSB 0xF8

 #define BMP180_OSS    2

 // Structure for sensor data
 struct bmpdata
  {
   int16_t ac1;
   int16_t ac2;
   int16_t ac3;
   uint16_t ac4;
   uint16_t ac5;
   uint16_t ac6;
   int16_t b1;
   int16_t b2;
   int16_t mb;
   int16_t mc;
   int16_t md;
   int32_t ut;
   int32_t up;
   int32_t temperature;
   int32_t pressure;
  };

 // Functions
 int bmpInit( struct bmpdata* d );
 int bmpRead( struct bmpdata* d );

#endif
