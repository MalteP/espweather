// #############################################################################
// #                        ESP8266 WiFi Weather Sensor                        #
// #############################################################################
// # sensors.h - Convert sensor data to string                                 #
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

#ifndef SENSORS_H
 #define SENSORS_H

 // Macros for sensorsRead() first_read variable
 #define SENSORS_FIRST_READ_FALSE 0
 #define SENSORS_FIRST_READ_TRUE  1

 // Macros for internal sensor_status
 #define SENSORS_STATUS_NOSENSOR -1
 #define SENSORS_STATUS_FOUND     0
 #define SENSORS_STATUS_FAILED    1
 #define SENSORS_STATUS_OK        2
 #define SENSORS_STATUS_PENDING   3

 // Convert return code from sensor functions to internal status
 // notice that SENSOR_RTN_FAILED (-1) / _OK (0) / _PENDING (1) is equivalent
 // to internal status SENSORS_STATUS_FAILED (1) / _OK (2) / _PENDING (3)
 #define SENSOR_TO_SENSORS_INCREMENT 2

 // Macros for return value of sensorsRead()
 #define SENSORS_RETURN_FAILED -1
 #define SENSORS_RETURN_OK      0
 #define SENSORS_RETURN_PENDING 1

 struct sensorlist
  {
   char *name;
   int (*func_init)(void*);
   int (*func_read)(void*);
   void *sensor_struct;
   int sensor_status;
   int *has_temperature;
   int32_t *val_temperature;
   int dpow_temperature;
   int *has_humidity;
   uint32_t *val_humidity;
   int dpow_humidity;
   int *has_pressure;
   uint32_t *val_pressure;
   int dpow_pressure;
  };

 void sensorsInit( void );
 int sensorsRead( int first_read );
 int sensorUseable( struct sensorlist *sensor, int v );
 char* temperatureToString( void );
 char* humidityToString( void );
 char* pressureToString( void );
 char* batteryVoltageToString( void );
 char* rssiToString( void );
 int sensors_sprintf( char *buf, int value, int dec_pow );

#endif
