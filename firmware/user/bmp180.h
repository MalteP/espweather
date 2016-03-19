// #############################################################################
// #                        ESP8266 WiFi Weather Sensor                        #
// #############################################################################
// # bmp180.h - Functions for BMP180 barometer                                 #
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

#ifndef BMP180_H
 #define BMP180_H

 #define BMP180_ADDR   0x77
 #define BMP180_ADDR_R ((BMP180_ADDR<<1)|1)
 #define BMP180_ADDR_W (BMP180_ADDR<<1)
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
   int32_t t;
   int32_t p;
  };

 // Functions
 int bmpInit( struct bmpdata* d );
 int bmpReadSensor( struct bmpdata* d );
 int16_t bmpReadRegister16( uint8_t addr );
 int32_t bmpReadRegister24( uint8_t addr );
 int bmpWriteRegister8( uint8_t addr, uint8_t value );

#endif
