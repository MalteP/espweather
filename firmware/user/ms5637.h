// #############################################################################
// #                        ESP8266 WiFi Weather Sensor                        #
// #############################################################################
// # ms5637.h - Functions for MS5637 barometer                                 #
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

#ifndef MS5637_H
 #define MS5637_H

 #define MS5637_ADDR       0x76

 #define MS5637_RESET      0x1E
 #define MS5637_CONVERT_D1 0x40
 #define MS5637_CONVERT_D2 0x50
 #define MS5637_ADC_READ   0x00
 #define MS5637_PROM_READ  0xA0

 #define MS5637_OSR_256    0x00 // 0
 #define MS5637_OSR_512    0x02 // 1
 #define MS5637_OSR_1024   0x04 // 2
 #define MS5637_OSR_2048   0x06 // 3
 #define MS5637_OSR_4096   0x08 // 4
 #define MS5637_OSR_8192   0x0A // 5

 #define MS5637_OSR_PRES   MS5637_OSR_2048
 #define MS5637_OS_PRES    3

 #define MS5637_OSR_TEMP   MS5637_OSR_2048
 #define MS5637_OS_TEMP    3

 // Enable CRC check of calibration data
 #define MS5637_ENABLE_CRC_CHECK

 // Enable 2nd order temperature compensation
 #define MS5637_ENABLE_COMPENSATION

 // Structure for sensor data
 struct msdata
  {
   uint16_t c[8];
   uint32_t d1;
   uint32_t d2;
   int32_t t;
   int32_t p;
  };

 // Functions
 int msInit( struct msdata* d );
 int msReadSensor( struct msdata* d );
 #ifdef MS5637_ENABLE_CRC_CHECK
 uint16_t msCheckCRC( uint16_t prom[] );
 #endif

#endif
