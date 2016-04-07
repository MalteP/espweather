// #############################################################################
// #                        ESP8266 WiFi Weather Sensor                        #
// #############################################################################
// # main.c - Main program                                                     #
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
#include "httpd.h"
#include "config.h"
#include "httpdespfs.h"
#include "cgisensor.h"
#include "cgibuild.h"
#include "cgiwifi.h"
#include "cgiconfig.h"
#include "stdout.h"
#include "auth.h"
#include "espfs.h"
#include "battery.h"
#include "sensors.h"
#include "push.h"


// Password authentication for admin interface
int myPassFn(HttpdConnData *connData, int no, char *user, int userLen, char *pass, int passLen)
 {
  if(no==0)
   {
    os_strncpy(user, configGet()->admin_user, userLen);
    os_strncpy(pass, configGet()->admin_pass, passLen);
    return 1;
   }
  return 0;
 }


// URLs for esphttpd
HttpdBuiltInUrl builtInUrls[]=
 {
  {"/", cgiRedirect, "/index.html"},
  {"/sensors.cgi", cgiSensor, NULL},
  {"/admin*", authBasic, myPassFn},
  {"/admin/", cgiRedirect, "/admin/index.html"},
  {"/admin/buildinfo.cgi", cgiBuildInfo, NULL},
  {"/admin/wifiscan.cgi", cgiWiFiScan, NULL},
  {"/admin/wifisettings.cgi", cgiWiFiSettings, NULL},
  {"/admin/sensorconfig.cgi", cgiSensorConfig, NULL},
  {"/admin/userconfig.cgi", cgiUserConfig, NULL},
  {"*", cgiEspFsHook, NULL}, //Catch-all cgi function for the filesystem
  {NULL, NULL, NULL}
 };


// Main routine, initialize sensors, webserver and push data
void ICACHE_FLASH_ATTR user_init(void)
 {
  // Init some stuff
  stdoutInit();
  batteryInit();
  configInit();
  configLoad();

  // Measure battery voltage
  batteryMeasureVoltage();
  if(!batteryCheckVoltage())
   {
    // Voltage too low, go to sleep mode!
    sleepmode();
   }

  // 0x40200000 is the base address for spi flash memory mapping, ESPFS_POS is the position
  // where image is written in flash that is defined in Makefile.
  espFsInit((void*)(0x40200000 + ESPFS_POS));
  httpdInit(builtInUrls, 80);

  os_printf("Ready\n");

  // Init sensors
  sensorsInit();

  // Check if wifi mode is correct
  configCheckWifiMode();

  if(configGet()->sensor_mode==MODE_NORMAL)
   {
    // Normal mode
    pushSensorData();
   } else {
    // Configuration mode
    statusLed(LED_BLINK);
   }

 }
