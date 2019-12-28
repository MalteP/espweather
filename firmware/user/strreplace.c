// #############################################################################
// #                        ESP8266 WiFi Weather Sensor                        #
// #############################################################################
// # strreplace.c - Replace substring                                          #
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
#include "config.h"


// This function will replace a substring with a replacement inside a string
bool ICACHE_FLASH_ATTR strreplace( char* str, char* sub, char* rep, unsigned int len )
 {
  char* temp;
  char* start;
  char* remaining;
  unsigned int lsub;
  unsigned int lrep;
  // Length of substring and replacement
  lsub = os_strlen(sub);
  lrep = os_strlen(rep);
  if(lsub==0) return false;
  // Find substring
  start = os_strstr(str, sub);
  if(start==NULL) return false;
  // Check total length
  if(os_strlen(str)-lsub+lrep+1>len) return false;
  // Position of remaining string
  remaining = start + lsub;
  // Alloc temporary buffer and copy
  temp=os_malloc(len);
  if(temp==NULL) return false;
  os_strcpy(temp, remaining);
  // Replace substring
  os_strcpy(start, rep);
  // Add temp string and free memory
  os_strcat(str, temp);
  os_free(temp);
  return true;
}
