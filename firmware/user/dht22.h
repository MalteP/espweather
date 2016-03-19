// #############################################################################
// #                        ESP8266 WiFi Weather Sensor                        #
// #############################################################################
// # dht22.h - Functions for DHT22 temperature / humidity sensor               #
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

#ifndef DHT22_H
 #define DHT22_H

 #define DHT22_MUX PERIPHS_IO_MUX_GPIO2_U
 #define DHT22_FUNC FUNC_GPIO2
 #define DHT22_GPIO 2

 // Start pulse (1000us)
 #define DHT22_T_START_US 1000

 // Bus master release time (20-200us)
 #define DHT22_T_RELEASE_US 40

 // Response pulse max time (75-85us)
 #define DHT22_T_RESPONSE_US 85

 // Bit 0 & 1 low time (48-55us)
 #define DHT22_T_LOW 55

 // Bit 0 high time (22-30us)
 #define DHT22_T_HIGH_0 32

 // Bit 1 high time (68-75us)
 #define DHT22_T_HIGH_1 75

 // Structure for sensor data
 struct dhtdata
  {
   uint8_t rawdata[5];
   int16_t temperature;
   int16_t humidity;
   BOOL valid;
  };

 // Functions
 int dht22Init( void );
 int dht22Read( struct dhtdata* d );

#endif
