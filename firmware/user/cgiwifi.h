// #############################################################################
// #                        ESP8266 WiFi Weather Sensor                        #
// #############################################################################
// # cgiwifi.h - CGI for "Network settings" tab                                #
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

#ifndef CGIWIFI_H
 #define CGIWIFI_H

 #include "httpd.h"

 // WiFi access point data
 typedef struct
  {
   char ssid[32];
   int rssi;
   char enc;
  } ApData;

 // Scan result
 typedef struct
  {
   char scanInProgress; // if 1, don't access the underlying stuff from the webpage.
   ApData **apData;
   int noAps;
  } ScanResultData;

 #define CONNTRY_IDLE 0
 #define CONNTRY_WORKING 1
 #define CONNTRY_SUCCESS 2
 #define CONNTRY_FAIL 3

 int cgiWiFiScan(HttpdConnData *connData);
 int cgiWiFiConnect(HttpdConnData *connData);
 int cgiWiFiSetMode(HttpdConnData *connData);
 int cgiWiFiConnStatus(HttpdConnData *connData);
 int cgiWiFiSettings(HttpdConnData *connData);

#endif
