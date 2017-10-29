// #############################################################################
// #                        ESP8266 WiFi Weather Sensor                        #
// #############################################################################
// # i2c-common.c - Common I2C read & write routines                           #
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

#include <esp8266.h>
#include "i2c-common.h"


void i2cInit( void )
 {
  // Disable GPIO interrupts
  ETS_GPIO_INTR_DISABLE();

  // Set pin function to GPIO
  PIN_FUNC_SELECT(I2C_SDA_MUX, I2C_SDA_FUNC);
  PIN_FUNC_SELECT(I2C_SCL_MUX, I2C_SCL_FUNC);

  // Data as open drain
  GPIO_REG_WRITE(GPIO_PIN_ADDR(GPIO_ID_PIN(I2C_SDA_PIN)), GPIO_REG_READ(GPIO_PIN_ADDR(GPIO_ID_PIN(I2C_SDA_PIN))) | GPIO_PIN_PAD_DRIVER_SET(GPIO_PAD_DRIVER_ENABLE));
  GPIO_REG_WRITE(GPIO_ENABLE_ADDRESS, GPIO_REG_READ(GPIO_ENABLE_ADDRESS) | (1 << I2C_SDA_PIN));

  // Clock as open drain
  GPIO_REG_WRITE(GPIO_PIN_ADDR(GPIO_ID_PIN(I2C_SCL_PIN)), GPIO_REG_READ(GPIO_PIN_ADDR(GPIO_ID_PIN(I2C_SCL_PIN))) | GPIO_PIN_PAD_DRIVER_SET(GPIO_PAD_DRIVER_ENABLE));
  GPIO_REG_WRITE(GPIO_ENABLE_ADDRESS, GPIO_REG_READ(GPIO_ENABLE_ADDRESS) | (1 << I2C_SCL_PIN));

  // Enable GPIO interrupts again
  ETS_GPIO_INTR_ENABLE();

  // Set ports
  I2C_SDA_HIGH();
  I2C_SCK_HIGH();
 }

// Write command to device
int ICACHE_FLASH_ATTR i2cWriteCmd( uint8_t device, uint8_t value, uint8_t sendstop )
 {
  i2cSendStart();
  // Write address
  if(i2cWriteByte(device<<1))
   {
    i2cSendStop();
    return -1;
   }
  // Write value
  if(i2cWriteByte(value))
   {
    i2cSendStop();
    return -1;
   }
  if(sendstop!=I2C_NO_STOP) i2cSendStop();
  return 0;
 }


// Write 8bit register via i2c
int ICACHE_FLASH_ATTR i2cWriteRegister8( uint8_t device, uint8_t addr, uint8_t value )
 {
  if(i2cWriteCmd(device, addr, I2C_NO_STOP)!=0)
   {
    return -1;
   }
  // Write value
  if(i2cWriteByte(value))
   {
    i2cSendStop();
    return -1;
   }
  i2cSendStop();
  return 0;
 }


// Read 16bit register via i2c
int16_t ICACHE_FLASH_ATTR i2cReadRegister16( uint8_t device, uint8_t value )
 {
  int16_t data;
  if(i2cWriteCmd(device, value, I2C_NO_STOP)!=0)
   {
    return -1;
   }
  // Repeated start
  i2cSendStart();
  // Sensor read address
  if(i2cWriteByte((device<<1)|1))
   {
    i2cSendStop();
    return -1;
   }
  // Read and return data
  data = i2cReadByte(1);
  data <<= 8;
  data |= i2cReadByte(0);
  i2cSendStop();
  return data;
 }


// Read 24bit register via i2c
int32_t ICACHE_FLASH_ATTR i2cReadRegister24( uint8_t device, uint8_t value )
 {
  int32_t data;
  if(i2cWriteCmd(device, value, I2C_NO_STOP)!=0)
   {
    return -1;
   }
  // Repeated start
  i2cSendStart();
  // Sensor read address
  if(i2cWriteByte((device<<1)|1))
   {
    i2cSendStop();
    return -1;
   }
  // Read and return data
  data = i2cReadByte(1);
  data <<= 8;
  data |= i2cReadByte(1);
  data <<= 8;
  data |= i2cReadByte(0);
  i2cSendStop();
  return data;
 }

// Send I2C start condition
int i2cSendStart( void )
 {
  I2C_SCK_HIGH();
  I2C_SDA_HIGH();
  I2C_DELAY();
  I2C_SDA_LOW();
  I2C_DELAY();
  I2C_SCK_LOW();
  I2C_DELAY();
 }

// Send I2C stop condition
int i2cSendStop( void )
 {
  I2C_SDA_LOW();
  I2C_DELAY();
  I2C_SCK_HIGH();
  I2C_DELAY();
  I2C_SDA_HIGH();
  I2C_DELAY();
 }

// Write byte to slave
int i2cWriteByte( int byte )
 {
  uint8 bit;
  // Write data
  for(bit=0;bit<8;bit++)
   {
    if(byte&0x80)
     {
      I2C_SDA_HIGH();
     } else {
      I2C_SDA_LOW();
     }
    I2C_DELAY();
    I2C_SCK_HIGH();
    I2C_DELAY();
    I2C_SCK_LOW();
    I2C_DELAY();
    byte <<= 1;
   }
  // Read ACK from slave
  I2C_SDA_HIGH();
  I2C_DELAY();
  I2C_SCK_HIGH();
  I2C_DELAY();
  bit = I2C_SDA_READ();
  I2C_SCK_LOW();
  I2C_DELAY();
  return bit;
 }

// Read byte from slave
int i2cReadByte( int ack )
 {
  uint8 byte = 0;
  uint8 bit;
  // Read data
  for(bit=0;bit<8;bit++)
   {
    I2C_SDA_HIGH();
    I2C_DELAY();
    I2C_SCK_HIGH();
    I2C_DELAY();
    byte <<= 1;
    byte |= I2C_SDA_READ();
    I2C_SCK_LOW();
    I2C_DELAY();
   }
  // Send ACK/NACK to slave
  if(ack)
   {
    I2C_SDA_LOW();
   } else {
    I2C_SDA_HIGH();
   }
  I2C_DELAY();
  I2C_SCK_HIGH();
  I2C_DELAY();
  I2C_SCK_LOW();
  I2C_DELAY();
  return byte;
 }
