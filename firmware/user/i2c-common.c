// #############################################################################
// #                        ESP8266 WiFi Weather Sensor                        #
// #############################################################################
// # i2c-common.c - Common I2C read & write routines                           #
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

#include <esp8266.h>
#include <i2c/i2c.h>
#include "i2c-common.h"


// Write command to device
int ICACHE_FLASH_ATTR i2cWriteCmd( uint8_t device, uint8_t value, uint8_t sendstop )
 {
  i2c_start();
  // Write address
  i2c_writeByte(device<<1);
  if (!i2c_check_ack())
   {
    i2c_stop();
    return -1;
   }
  // Write value
  i2c_writeByte(value);
  if (!i2c_check_ack())
   {
    i2c_stop();
    return -1;
   }
  if(sendstop!=I2C_NO_STOP) i2c_stop();
  return 0;
 }


// Write 8bit register via i2c
int ICACHE_FLASH_ATTR i2cWriteRegister8( uint8_t device, uint8_t addr, uint8_t value )
 {
  i2cWriteCmd( device, addr, I2C_NO_STOP );
  // Write value
  i2c_writeByte(value);
  if (!i2c_check_ack())
   {
    i2c_stop();
    return -1;
   }
  i2c_stop();
  return 0;
 }


// Read 16bit register via i2c
int16_t ICACHE_FLASH_ATTR i2cReadRegister16( uint8_t device, uint8_t value, uint8_t sendstop )
 {
  int16_t data;
  if(i2cWriteCmd(device, value, sendstop)!=0)
   {
    return -1;
   }
  // (Repeated) start
  i2c_start();
  // Sensor read address
  i2c_writeByte((device<<1)|1);
  if (!i2c_check_ack())
   {
    i2c_stop();
    return -1;
   }
  // Read and return data
  data = i2c_readByte()<<8;
  i2c_send_ack(1);
  data |= i2c_readByte();
  i2c_stop();
  return data;
 }


// Read 24bit register via i2c
int32_t ICACHE_FLASH_ATTR i2cReadRegister24( uint8_t device, uint8_t value, uint8_t sendstop )
 {
  int32_t data;
  if(i2cWriteCmd(device, value, sendstop)!=0)
   {
    return -1;
   }
  // (Repeated) start
  i2c_start();
  // Sensor read address
  i2c_writeByte((device<<1)|1);
  if (!i2c_check_ack())
   {
    i2c_stop();
    return -1;
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
