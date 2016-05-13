// #############################################################################
// #                        ESP8266 WiFi Weather Sensor                        #
// #############################################################################
// # battery.h - Battery voltage measurement                                   #
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

#ifndef BATTERY_H
 #define BATTERY_H

 // The reference voltage seems to be a bit off on some ESP
 // so you may need to add a offset voltage (mV) here
 #define BATTERY_VOLTAGE_OFFSET_MV -100

 // Set the voltage (mV) to whatever your battery
 // chemistry used may be discharged to
 #define BATTERY_VOLTAGE_LIMIT_MV  3000

 void batteryInit( void );
 void batteryMeasureVoltage( void );
 unsigned int batteryGetVoltage( void );
 int batteryCheckVoltage( void );

#endif
