// #############################################################################
// #                        ESP8266 WiFi Weather Sensor                        #
// #############################################################################
// # sensors.h - Convert sensor data to string                                 #
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

#ifndef SENSORS_H
 #define SENSORS_H

 void sensorsInit( void );
 void sensorsRead( void );
 int sensorsDone( void );
 char* temperatureToString( void );
 char* humidityToString( void );
 char* pressureToString( void );
 char* batteryVoltageToString( void );
 char* rssiToString( void );

#endif
