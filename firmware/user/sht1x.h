// #############################################################################
// #                        ESP8266 WiFi Weather Sensor                        #
// #############################################################################
// # sht1x.h - Functions for SHT11/12/15 temperature / humidity sensor         #
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

#ifndef SHT1x_H
 #define SHT1x_H

 // Ports & registers
 #define SHT_DATA_MUX PERIPHS_IO_MUX_GPIO4_U
 #define SHT_DATA_FUNC FUNC_GPIO4
 #define SHT_DATA_PIN 4

 #define SHT_CLK_MUX PERIPHS_IO_MUX_GPIO5_U
 #define SHT_CLK_FUNC FUNC_GPIO5
 #define SHT_CLK_PIN 5

 // Use CRC?
 #define SHT_USE_CRC

 // Use floating point calculations
 #define USE_FLOAT_CALC

 // CRC Lookup table
 #ifdef SHT_USE_CRC
 static const uint8_t sht_crc_table[] =
  {
      0,  49,  98,  83, 196, 245, 166, 151, 185, 136, 219, 234, 125,  76,  31,  46,
     67, 114,  33,  16, 135, 182, 229, 212, 250, 203, 152, 169,  62,  15,  92, 109,
    134, 183, 228, 213,  66, 115,  32,  17,  63,  14,  93, 108, 251, 202, 153, 168,
    197, 244, 167, 150,   1,  48,  99,  82, 124,  77,  30,  47, 184, 137, 218, 235,
     61,  12,  95, 110, 249, 200, 155, 170, 132, 181, 230, 215,  64, 113,  34,  19,
    126,  79,  28,  45, 186, 139, 216, 233, 199, 246, 165, 148,   3,  50,  97,  80,
    187, 138, 217, 232, 127,  78,  29,  44,   2,  51,  96,  81, 198, 247, 164, 149,
    248, 201,  54, 171,  60,  13,  94, 111,  65, 112,  35,  18, 133, 180, 231, 214,
    122,  75,  24,  41, 190, 143, 220, 237, 195, 242, 161, 144,   7,  54, 101,  84,
     57,   8,  91, 106, 253, 204, 159, 174, 128, 177, 226, 211,  68, 117,  38,  23,
    252, 205, 158, 175,  56,   9,  90, 107,  69, 116,  39,  22, 129, 176, 227, 210,
    191, 142, 221, 236, 123,  74,  25,  40,   6,  55, 100,  85, 194, 243, 160, 145,
     71, 118,  37,  20, 131, 178, 225, 208, 254, 207, 156, 173,  58,  11,  88, 105,
      4,  53, 102,  87, 192, 241, 162, 147, 189, 140, 223, 238, 121,  72,  27,  42,
    193, 240, 163, 146,   5,  52, 103,  86, 120,  73,  26,  43, 188, 141, 222, 239,
    130, 179, 224, 209,  70, 119,  36,  21,  59,  10,  89, 104, 255, 206, 157, 172
  };
 #endif

 // Compensation values
 #define SHT_TEMPERATURE_D1 -39.6 // 3V, 14bit
 #define SHT_TEMPERATURE_D2  0.01
 #define SHT_HUMIDITY_C1    -2.0468
 #define SHT_HUMIDITY_C2     0.0367
 #define SHT_HUMIDITY_C3    -0.0000015955
 #define SHT_HUMIDITY_T1     0.01
 #define SHT_HUMIDITY_T2     0.00008

 // Port macros
 #define SHT_DATA_HIGH() gpio_output_set(1 << SHT_DATA_PIN, 0, 1 << SHT_DATA_PIN, 0)
 #define SHT_DATA_LOW()  gpio_output_set(0, 1 << SHT_DATA_PIN, 1 << SHT_DATA_PIN, 0)
 #define SHT_CLK_HIGH()  gpio_output_set(1 << SHT_CLK_PIN,  0, 1 << SHT_CLK_PIN,  0)
 #define SHT_CLK_LOW()   gpio_output_set(0,  1 << SHT_CLK_PIN, 1 << SHT_CLK_PIN,  0)
 #define SHT_READ()      GPIO_INPUT_GET(GPIO_ID_PIN(SHT_DATA_PIN))

 // Timings
 #define SHT_TSCK_NS          125
 #define SHT_DATA_VALID_NS    250
 #define SHT_MEASUREMENT8_MS   20
 #define SHT_MEASUREMENT12_MS  80
 #define SHT_MEASUREMENT14_MS 320

 // Commands
 #define SHT_CMD_MEASURE_TEMPERATURE 0x03
 #define SHT_CMD_MEASURE_HUMIDITY    0x05
 #define SHT_CMD_READ_STATUS_REG     0x07
 #define SHT_CMD_WRITE_STATUS_REG    0x06
 #define SHT_CMD_SOFT_RESET          0x1E

 // Structure for sensor data
 struct shtdata
  {
   int32_t temperature;
   uint32_t humidity;
  };

 int shtInit( struct shtdata* d );
 int shtRead( struct shtdata* d );

 // Low level functions
 void shtReset( void );
 void shtStart( void );
 void shtSendAck( void );
 void shtSkipAck( void );
 uint8_t shtReadAck( void );
 uint8_t shtRead8( void );
 uint16_t shtRead16( void );
 void shtWrite8( uint8_t data );
 uint8_t shtWaitForConversion( uint16_t time );
 void shtDelay( void );
 int16_t shtConvertTemperature( uint16_t temperature_raw );
 int16_t shtConvertHumidity( uint16_t humidity_raw, int16_t temperature );
 #ifdef SHT_USE_CRC
 uint8_t shtReverseByte( uint8_t a );
 uint8_t shtCalculateCRC( uint8_t start, uint8_t byte0, uint8_t byte1, uint8_t byte2 );
 #endif

#endif
