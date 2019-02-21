// #############################################################################
// #                        ESP8266 WiFi Weather Sensor                        #
// #############################################################################
// # i2c-common.h - Common I2C read & write routines                           #
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

#ifndef I2C_COMMON_H
 #define I2C_COMMON_H

 // Ports & registers
 #define I2C_SDA_MUX  PERIPHS_IO_MUX_GPIO4_U
 #define I2C_SDA_FUNC FUNC_GPIO4
 #define I2C_SDA_PIN  4

 #define I2C_SCL_MUX  PERIPHS_IO_MUX_GPIO5_U
 #define I2C_SCL_FUNC FUNC_GPIO5
 #define I2C_SCL_PIN  5

 // Port macros
 #define I2C_SDA_HIGH() gpio_output_set(1 << I2C_SDA_PIN, 0, 1 << I2C_SDA_PIN, 0)
 #define I2C_SDA_LOW()  gpio_output_set(0, 1 << I2C_SDA_PIN, 1 << I2C_SDA_PIN, 0)
 #define I2C_SCK_HIGH() gpio_output_set(1 << I2C_SCL_PIN,  0, 1 << I2C_SCL_PIN,  0)
 #define I2C_SCK_LOW()  gpio_output_set(0,  1 << I2C_SCL_PIN, 1 << I2C_SCL_PIN,  0)
 #define I2C_SDA_READ() GPIO_INPUT_GET(GPIO_ID_PIN(I2C_SDA_PIN))

 // Timings
 #define I2C_DELAY_US 10
 #define I2C_DELAY()  os_delay_us(I2C_DELAY_US)

 // Send stop in i2cWriteCmd()
 #define I2C_NO_STOP   0
 #define I2C_SEND_STOP 1

 // Functions
 void i2cInit( void );
 int i2cWriteCmd( uint8_t device, uint8_t value, uint8_t sendstop );
 int i2cWriteRegister8( uint8_t device, uint8_t addr, uint8_t value );
 uint8_t i2cReadRegister8( uint8_t device, uint8_t value );
 uint16_t i2cReadRegister16( uint8_t device, uint8_t value );
 uint16_t i2cReadRegister16LE( uint8_t device, uint8_t value );
 uint32_t i2cReadRegister24( uint8_t device, uint8_t value );

 // Low level functions
 int i2cSendStart( void );
 int i2cSendStop( void );
 int i2cWriteByte( int byte );
 int i2cReadByte( int ack );

#endif
