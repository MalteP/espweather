// #############################################################################
// #                        ESP8266 WiFi Weather Sensor                        #
// #############################################################################
// # sensors.c - Convert sensor data to string                                 #
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
#include "sensors.h"
#include "dht22.h"
#include "sht1x.h"
#include "sht3x.h"
#include "bmp180.h"
#include "ms5637.h"
#include "battery.h"

struct shtdata sht1x;
struct sht3data sht3x;
struct dhtdata dht22;
struct msdata ms5637;
struct bmpdata bmp180;

int sht1x_available = -1;
int sht3x_available = -1;
int ms5637_available = -1;
int bmp180_available = -1;

int temphum_valid = -1;
int pressure_valid = -1;

int read_limit = 0;
int in_progress = 0;

char temperature[8];
char humidity[8];
char pressure[8];
char battery[8];
char rssi[8];

void sensorsReadCb( void *arg );
void readLimitCb( void *arg );


// Initialize sensors
void ICACHE_FLASH_ATTR sensorsInit( void )
 {
  static ETSTimer sensorsTimer;
  sht3x_available = sht3Init(&sht3x);
  sht1x_available = shtInit(&sht1x);
  dht22Init(&dht22);
  ms5637_available = msInit(&ms5637);
  bmp180_available = bmpInit(&bmp180);
  os_printf("Sensors: SHT3x=%c, SHT1x=%c, DHT=?, MS=%c, BMP=%c\n", (sht3x_available==0?'y':'n'), (sht1x_available==0?'y':'n'), (ms5637_available==0?'y':'n'), (bmp180_available==0?'y':'n'));
  temperature[0]='\0';
  humidity[0]='\0';
  pressure[0]='\0';
  battery[0]='\0';
  // Because GPIO2 (DHT22 pin) is alternative UART TX in bootloader mode, the sensor seems
  // to get a little bit confused and needs some delay before first read after boot...
  if(sht3x_available!=0&&sht1x_available!=0)
   {
    in_progress = 1;
    os_timer_disarm(&sensorsTimer);
    os_timer_setfn(&sensorsTimer, sensorsReadCb, NULL);
    os_timer_arm(&sensorsTimer, 500, 0);
   } else {
    sensorsRead();
   }
 }


// First read a few hundred milliseconds delayed
void ICACHE_FLASH_ATTR sensorsReadCb( void *arg )
 {
  sensorsRead();
 }


// Read available sensors
void ICACHE_FLASH_ATTR sensorsRead( void )
 {
  static ETSTimer limitTimer;
  if(read_limit!=0) return;
  read_limit = 1;
  in_progress = 1;
  temphum_valid = -1;
  pressure_valid = -1;
  if(sht3x_available==0)
   {
    temphum_valid = sht3Read(&sht3x);
   } else {
    if(sht1x_available==0)
     {
      temphum_valid = shtRead(&sht1x);
     } else {
      temphum_valid = dht22Read(&dht22);
     }
   }
  if(ms5637_available==0)
   {
    pressure_valid = msReadSensor(&ms5637);
   } else {
    if(bmp180_available==0)
     {
      pressure_valid = bmpReadSensor(&bmp180);
     }
   }
  in_progress = 0;
  // Reading the sensor is only allowed every 5 sec.
  os_timer_disarm(&limitTimer);
  os_timer_setfn(&limitTimer, readLimitCb, NULL);
  os_timer_arm(&limitTimer, 5000, 0);
 }


// Reset flag
void ICACHE_FLASH_ATTR readLimitCb( void *arg )
 {
  read_limit = 0;
 }


// Wait for sensors?
int sensorsDone( void )
 {
  return in_progress;
 }


// Convert temperature value to string
char* ICACHE_FLASH_ATTR temperatureToString( void )
 {
  temperature[0]='\0';
  if(temphum_valid!=0) goto end;
  if(sht3x_available==0)
   {
    sensors_sprintf(temperature, sht3x.temperature, 2);
   } else {
    if(sht1x_available==0)
     {
      sensors_sprintf(temperature, sht1x.temperature, 1);
     } else {
      sensors_sprintf(temperature, dht22.temperature, 1);
     }
   }
  end:
  return temperature;
 }


// Convert humidity value to string
char* ICACHE_FLASH_ATTR humidityToString( void )
 {
  humidity[0]='\0';
  if(temphum_valid!=0) goto end;
  if(sht3x_available==0)
   {
     sensors_sprintf(humidity, sht3x.humidity, 2);
   } else {
    if(sht1x_available==0)
     {
      sensors_sprintf(humidity, sht1x.humidity, 1);
     } else {
      sensors_sprintf(humidity, dht22.humidity, 1);
     }
   }
  end:
  return humidity;
 }


// Convert barometric pressure to string
char* ICACHE_FLASH_ATTR pressureToString( void )
 {
  pressure[0]='\0';
  if(pressure_valid!=0) goto end;
  if(ms5637_available==0)
   {
    sensors_sprintf(pressure, ms5637.pressure, 2);
   } else {
    if(bmp180_available==0)
     {
      sensors_sprintf(pressure, bmp180.pressure, 2);
     }
   }
  end:
  return pressure;
 }


// Convert battery voltage to string
char* ICACHE_FLASH_ATTR batteryVoltageToString( void )
 {
  int battery_voltage = batteryGetVoltage();
  battery[0]='\0';
  sensors_sprintf(battery, battery_voltage, 3);
  return battery;
 }

// Convert WiFi RSSI to string
char* ICACHE_FLASH_ATTR rssiToString( void )
 {
  int rssi_value = wifi_station_get_rssi();
  rssi[0]='\0';
  if(rssi_value!=31) // 31 = Failure
   {
    sensors_sprintf(rssi, rssi_value, 0);
   }
  return rssi;
 }

int ICACHE_FLASH_ATTR sensors_sprintf( char *buf, int value, int dec_pow )
 {
  char *sign = (value<0?"-":"");
  switch(dec_pow)
   {
    case 0: return os_sprintf(buf, "%d", value);
    case 1: return os_sprintf(buf, "%s%d.%d", sign, abs(value/10), abs(value%10));
    case 2: return os_sprintf(buf, "%s%d.%02d", sign, abs(value/100), abs(value%100));
    case 3: return os_sprintf(buf, "%s%d.%03d", sign, abs(value/1000), abs(value%1000));
   }
  return -1;
 }
