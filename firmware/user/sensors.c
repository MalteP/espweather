// #############################################################################
// #                        ESP8266 WiFi Weather Sensor                        #
// #############################################################################
// # sensors.c - Convert sensor data to string                                 #
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

#include <esp8266.h>
#include "sensors.h"
#include "dht22.h"
#include "bmp180.h"
#include "sht1x.h"
#include "battery.h"

struct dhtdata dht22;
struct bmpdata bmp180;
struct shtdata sht;
char temperature[8];
char humidity[8];
char pressure[8];
char battery[8];


// Initialize sensors
void ICACHE_FLASH_ATTR sensorsInit( void )
 {
  dht22Init();
  shtInit(&sht);
  bmpInit(&bmp180);
  temperature[0]='\0';
  humidity[0]='\0';
  pressure[0]='\0';
  battery[0]='\0';
  sensorsRead();
 }


// Read available sensors
void ICACHE_FLASH_ATTR sensorsRead( void )
 {
  shtRead(&sht);
  // No SHT1x? Try to read DHT22
  if(sht.valid!=true)
   {
    dht22Read(&dht22);
   }
  bmpReadSensor(&bmp180);
 }


// Convert temperature value to string
char* ICACHE_FLASH_ATTR temperatureToString( void )
 {
  temperature[0]='\0';
  if(sht.valid==true)
   {
    os_sprintf(temperature, "%d.%d", (sht.temperature/10), abs(sht.temperature%10));
   } else if(dht22.valid==true)
    {
    os_sprintf(temperature, "%d.%d", (dht22.temperature/10), abs(dht22.temperature%10));
    }
  return temperature;
 }


// Convert humidity value to string
char* ICACHE_FLASH_ATTR humidityToString( void )
 {
  humidity[0]='\0';
  if(sht.valid==true)
   {
    os_sprintf(humidity, "%d.%d", (sht.humidity/10), abs(sht.humidity%10));
   } else if(dht22.valid==true)
    {
    os_sprintf(humidity, "%d.%d", (dht22.humidity/10), abs(dht22.humidity%10));
    }
  return humidity;
 }


// Convert barometric pressure to string
char* ICACHE_FLASH_ATTR pressureToString( void )
 {
  pressure[0]='\0';
  os_sprintf(pressure, "%ld.%02d", (bmp180.p/100), abs(bmp180.p%100));
  return pressure;
 }


// Convert battery voltage to string
char* ICACHE_FLASH_ATTR batteryVoltageToString( void )
 {
  int battery_voltage = batteryGetVoltage();
  battery[0]='\0';
  os_sprintf(battery, "%d.%03d", (battery_voltage/1000), abs(battery_voltage%1000));
  return battery;
 }
