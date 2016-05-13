// #############################################################################
// #                        ESP8266 WiFi Weather Sensor                        #
// #############################################################################
// # i2c-common.h - Common I2C read & write routines                           #
// #############################################################################
// #            Version: 1.1 - Compiler: esp-open-sdk 1.5.2 (Linux)            #
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

#ifndef I2C_COMMON_H
 #define I2C_COMMON_H

 // Send I2C stop before repeated start?
 #define I2C_NO_STOP   0
 #define I2C_SEND_STOP 1

 // Functions
 int i2cWriteCmd( uint8_t device, uint8_t value, uint8_t sendstop );
 int i2cWriteRegister8( uint8_t device, uint8_t addr, uint8_t value );
 int16_t i2cReadRegister16( uint8_t device, uint8_t value, uint8_t sendstop );
 int32_t i2cReadRegister24( uint8_t device, uint8_t value, uint8_t sendstop );

#endif
