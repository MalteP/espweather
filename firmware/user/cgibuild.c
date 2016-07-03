// #############################################################################
// #                        ESP8266 WiFi Weather Sensor                        #
// #############################################################################
// # cgibuild.c - CGI which returns build info for "System information" tab    #
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
#include "cgibuild.h"
#include "config.h"

// Send build info
int ICACHE_FLASH_ATTR cgiBuildInfo(HttpdConnData *connData)
 {
  char buff[1536];
  int len;
  // Check if connection exists
  if(connData->conn==NULL)
   {
    return HTTPD_CGI_DONE;
   }
  // HTTP Header
  httpdStartResponse(connData, 200);
  httpdHeader(connData, "Content-Type", "text/plain");
  httpdHeader(connData, "Pragma", "no-cache");
  httpdHeader(connData, "Expires", "0");
  httpdEndHeaders(connData);
  // AJAX data
  len=os_sprintf(buff, "espweather %s\nCompiled on %s, %s\n%s (GCC %s)\n", VERSTRING, __DATE__, __TIME__, HOSTSTRING, __VERSION__);
  httpdSend(connData, buff, len);
  // Return
  return HTTPD_CGI_DONE;
 }
