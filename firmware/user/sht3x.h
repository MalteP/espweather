// #############################################################################
// #                        ESP8266 WiFi Weather Sensor                        #
// #############################################################################
// # sht3x.h - Functions for SHT30/31/35 temperature / humidity sensor         #
// #############################################################################
// #            Version: 1.2 - Compiler: esp-open-sdk 1.5.2 (Linux)            #
// #  (c) 2015-2017 by Malte Pöggel - www.MALTEPOEGGEL.de - malte@poeggel.de   #
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

#ifndef SHT3x_H
 #define SHT3x_H

 #define SHT3x_ADDR           0x44 // Device address

 #define SHT3x_SRESET_MSB     0x30 // Softreset
 #define SHT3x_SRESET_LSB     0xA2
 #define SHT3x_MS_HIGH_MSB    0x24 // Measurement with no clock stretching, high repetability
 #define SHT3x_MS_HIGH_LSB    0x00
 #define SHT3x_MS_MED_MSB     0x24 // Measurement with no clock stretching, medium repetability
 #define SHT3x_MS_MED_LSB     0x0B
 #define SHT3x_MS_LOW_MSB     0x24 // Measurement with no clock stretching, low repetability
 #define SHT3x_MS_LOW_LSB     0x16
 #define SHT3x_MS_CS_HIGH_MSB 0x2C // Measurement with clock stretching, high repetability
 #define SHT3x_MS_CS_HIGH_LSB 0x06
 #define SHT3x_MS_CS_MED_MSB  0x2C // Measurement with clock stretching, medium repetability
 #define SHT3x_MS_CS_MED_LSB  0x0D
 #define SHT3x_MS_CS_LOW_MSB  0x2C // Measurement with clock stretching, low repetability
 #define SHT3x_MS_CS_LOW_LSB  0x10

 // Use CRC?
 #define SHT3_USE_CRC
 #define SHT3_CRC_POLYNOMIAL 0x31

 // Structure for sensor data
 struct sht3data
  {
   int32_t temperature;
   uint32_t humidity;
  };

 int sht3Init( struct sht3data* d );
 int sht3Read( struct sht3data* d );
 #ifdef SHT3_USE_CRC
 uint8_t sht3CalculateCRC( uint8_t start, uint16_t byte );
 #endif

#endif
