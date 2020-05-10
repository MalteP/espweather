// #############################################################################
// #                        ESP8266 WiFi Weather Sensor                        #
// #############################################################################
// # sensors.c - Convert sensor data to string                                 #
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

#include <esp8266.h>
#include "sensors.h"
#include "sensor-common.h"
#include "dht22.h"
#include "sht1x.h"
#include "sht3x.h"
#include "ms5637.h"
#include "bmp180.h"
#include "bmx280.h"
#include "battery.h"

struct shtdata sht1x;
struct sht3data sht3x;
struct dhtdata dht22;
struct msdata ms5637;
struct bmpdata bmp180;
struct bmx280data bmx280;

// List of all known sensors and their values
// It is impossible to detect the presence of DHT22 during initialization, so it will always be reported as found. 
// If you want to use temperature values of the pressure sensors, please comment out the DHT22 line.
struct sensorlist sensors[][10] =
 {
  { "SHT3x",  (void*)&sht3Init,   (void*)&sht3Read,   &sht3x,  -1, NULL, &(sht3x.temperature),  2, NULL,                &(sht3x.humidity),  2, NULL, NULL              , 0 },
  { "BMx280", (void*)&bmx280Init, (void*)&bmx280Read, &bmx280, -1, NULL, &(bmx280.temperature), 2, &bmx280.sensor_type, &(bmx280.humidity), 3, NULL, &(bmx280.pressure), 2 },
  { "SHT1x",  (void*)&shtInit,    (void*)&shtRead,    &sht1x,  -1, NULL, &(sht1x.temperature),  1, NULL,                &(sht1x.humidity),  1, NULL, NULL              , 0 },
  { "DHT22",  (void*)&dht22Init,  (void*)&dht22Read,  &dht22,  -1, NULL, &(dht22.temperature),  1, NULL,                &(dht22.humidity),  1, NULL, NULL              , 0 },
  { "MS5637", (void*)&msInit,     (void*)&msRead,     &ms5637, -1, NULL, &(ms5637.temperature), 2, NULL,                NULL,               0, NULL, &(ms5637.pressure), 2 },
  { "BMP180", (void*)&bmpInit,    (void*)&bmpRead,    &bmp180, -1, NULL, &(bmp180.temperature), 1, NULL,                NULL,               0, NULL, &(bmp180.pressure), 2 }
 };

// Do not read all sensors everytime sensorsRead() called. See timer below.
int read_limit = 0;
void readLimitCb( void *arg );


// Initialize sensors
void ICACHE_FLASH_ATTR sensorsInit( void )
 {
  int i;
  // Call init function for all known sensor types
  for(i=0; i<sizeof(sensors)/sizeof(*sensors); i++)
   {
    sensors[i]->sensor_status = (*(sensors[i]->func_init))(sensors[i]->sensor_struct);
    os_printf("Sensors: Init %s? %s\n", sensors[i]->name, (sensors[i]->sensor_status<SENSORS_STATUS_FOUND?"no":"yes"));
   }
 }


// Read available sensors
int ICACHE_FLASH_ATTR sensorsRead( int first_read )
 {
  static ETSTimer limitTimer;
  int i, v;
  int status;
  int rtn = SENSORS_RETURN_OK;
  // Limit does not apply for reading failed sensors again
  if(first_read!=SENSORS_FIRST_READ_FALSE)
   {
    if(read_limit!=0) return 1;
    read_limit = 1;
   }
  // Reset status on first try / reset status for failed sensors
  for(i=0; i<sizeof(sensors)/sizeof(*sensors); i++)
   {
    if(first_read!=SENSORS_FIRST_READ_FALSE)
     {
      // Reset all available sensors to found state
      if(sensors[i]->sensor_status>SENSORS_STATUS_FOUND) sensors[i]->sensor_status = SENSORS_STATUS_FOUND;
     } else {
      // Will read again sensors in read failed or read pending state
      if(sensors[i]->sensor_status==SENSORS_STATUS_FAILED||sensors[i]->sensor_status==SENSORS_STATUS_PENDING) sensors[i]->sensor_status = SENSORS_STATUS_FOUND;
     }
   }
   // Find and read sensors for all known values
   for(v=0; v<3; v++)
    {
     for(i=0; i<sizeof(sensors)/sizeof(*sensors); i++)
      {
       // Check if current sensor was found and can deliver needed value
       if(sensorUseable(sensors[i], v)!=0) continue;
       // Check if sensor needs to be read
       if(sensors[i]->sensor_status==SENSORS_STATUS_FOUND)
        {
         status = (*(sensors[i]->func_read))(sensors[i]->sensor_struct);
         os_printf("Sensors: Read %s %s\n", sensors[i]->name, (status<=SENSOR_RTN_FAILED?"failed":(status==SENSOR_RTN_OK?"ok":"pending")));
         // Translate return code from sensor function to internal status code
         sensors[i]->sensor_status = status + SENSOR_TO_SENSORS_INCREMENT;
         // If sensor read is in failed or pending state, change return code
         if(status!=SENSOR_RTN_OK)
          {
           if(status==SENSOR_RTN_PENDING)
            {
             rtn = SENSORS_RETURN_PENDING; // Read pending (should not increment failed counter)
            } else {
             rtn = SENSORS_RETURN_FAILED;  // Read failed
            }
          }
        }
       // Done, start with next value
       break;
      }
    }
  if(first_read!=SENSORS_FIRST_READ_FALSE)
   {
    // Reading is only allowed every 5 sec, timer will reset the read_limit flag
    os_timer_disarm(&limitTimer);
    os_timer_setfn(&limitTimer, readLimitCb, NULL);
    os_timer_arm(&limitTimer, 5000, 0);
   }
  return rtn;
 }


// Check sensor from list if found and suitable
int ICACHE_FLASH_ATTR sensorUseable( struct sensorlist *sensor, int v )
 {
  // Skip if not detected
  if(sensor->sensor_status<SENSORS_STATUS_FOUND) return -1;
  // Check if sensor provides value
  switch(v)
   {
    case 0: // Temperature
     if(sensor->val_temperature==NULL) return -1;
     if(sensor->has_temperature!=NULL)
      {
       if(*(sensor->has_temperature)==0) return -1;
      }
     break;
    case 1: // Humidity
     if(sensor->val_humidity==NULL) return -1;
     if(sensor->has_humidity!=NULL)
      {
       if(*(sensor->has_humidity)==0) return -1;
      }
     break;
    case 2: // Pressure
     if(sensor->val_pressure==NULL) return -1;
     if(sensor->has_pressure!=NULL)
      {
       if(*(sensor->has_pressure)==0) return -1;
      }
     break;
   }
  return 0;
 }
 

// Reset flag
void ICACHE_FLASH_ATTR readLimitCb( void *arg )
 {
  read_limit = 0;
 }


// Convert temperature value to string
char* ICACHE_FLASH_ATTR temperatureToString( void )
 {
  int i;
  static char temperature[8];
  temperature[0]='\0';
  for(i=0; i<sizeof(sensors)/sizeof(*sensors); i++)
   {
    if(sensorUseable(sensors[i], 0)==0&&sensors[i]->sensor_status==SENSORS_STATUS_OK)
     {
      sensors_sprintf(temperature, *(sensors[i]->val_temperature), sensors[i]->dpow_temperature);
      break;
     }
   }
  return temperature;
 }


// Convert humidity value to string
char* ICACHE_FLASH_ATTR humidityToString( void )
 {
  int i;
  static char humidity[8];
  humidity[0]='\0';
  for(i=0; i<sizeof(sensors)/sizeof(*sensors); i++)
   {
    if(sensorUseable(sensors[i], 1)==0&&sensors[i]->sensor_status==SENSORS_STATUS_OK)
     {
      sensors_sprintf(humidity, *(sensors[i]->val_humidity), sensors[i]->dpow_humidity);
      break;
     }
   }
  return humidity;
 }


// Convert barometric pressure to string
char* ICACHE_FLASH_ATTR pressureToString( void )
 {
  int i;
  static char pressure[8];
  pressure[0]='\0';
  for(i=0; i<sizeof(sensors)/sizeof(*sensors); i++)
   {
    if(sensorUseable(sensors[i], 2)==0&&sensors[i]->sensor_status==SENSORS_STATUS_OK)
     {
      sensors_sprintf(pressure, *(sensors[i]->val_pressure), sensors[i]->dpow_pressure);
      break;
     }
   }
  return pressure;
 }


// Convert battery voltage to string
char* ICACHE_FLASH_ATTR batteryVoltageToString( void )
 {
  static char battery[8];
  int battery_voltage = batteryGetVoltage();
  battery[0]='\0';
  sensors_sprintf(battery, battery_voltage, 3);
  return battery;
 }


// Convert WiFi RSSI to string
char* ICACHE_FLASH_ATTR rssiToString( void )
 {
  static char rssi[8];
  int rssi_value = wifi_station_get_rssi();
  rssi[0]='\0';
  if(rssi_value!=31) // 31 = Failure
   {
    sensors_sprintf(rssi, rssi_value, 0);
   }
  return rssi;
 }


// Format function for sensor data
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
