// #############################################################################
// #                        ESP8266 WiFi Weather Sensor                        #
// #############################################################################
// # cgisensor.c - CGI for main page of webinterface                           #
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
#include "cgisensor.h"
#include "sensors.h"


// Read current sensor values
int ICACHE_FLASH_ATTR cgiSensor(HttpdConnData *connData)
 {
  char buff[1024];
  int len;
  httpdStartResponse(connData, 200);
  httpdHeader(connData, "Content-Type", "text/json");
  httpdHeader(connData, "Pragma", "no-cache");
  httpdHeader(connData, "Expires", "0");
  httpdEndHeaders(connData);
  sensorsRead(SENSORS_FIRST_READ_TRUE);
  len=os_sprintf(buff, "{\n \"temperature\": \"%s\",\n \"humidity\": \"%s\",\n \"barometer\": \"%s\",\n \"battery\": \"%s\",\n \"rssi\": \"%s\"\n}\n", temperatureToString(), humidityToString(), pressureToString(), batteryVoltageToString(), rssiToString());
  httpdSend(connData, buff, len);
  return HTTPD_CGI_DONE;
 }
