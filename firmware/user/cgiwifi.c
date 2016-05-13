// #############################################################################
// #                        ESP8266 WiFi Weather Sensor                        #
// #############################################################################
// # cgiwifi.c - CGI for "Network settings" tab                                #
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

// ** Parts of this code are based on Jeroen Domburgs work for esphttpd,      **
// ** which was licensed under the terms of beer-ware license:                **
// "THE BEER-WARE LICENSE" (Revision 42):
// Jeroen Domburg <jeroen@spritesmods.com> wrote this file. As long as you retain 
// this notice you can do whatever you want with this stuff. If we meet some day, 
// and you think this stuff is worth it, you can buy me a beer in return. 


#include <esp8266.h>
#include "cgiwifi.h"
#include "config.h"

// Static scan status storage.
static ScanResultData cgiWifiAps;

// Temp store for new ap info.
static struct station_config stconf;


// Callback the code calls when a wlan ap scan is done. Basically stores the result in the cgiWifiAps struct.
void ICACHE_FLASH_ATTR wifiScanDoneCb(void *arg, STATUS status)
 {
  int n;
  struct bss_info *bss_link = (struct bss_info *)arg;
  if (status!=OK)
   {
    cgiWifiAps.scanInProgress=0;
    return;
   }
  // Clear prev ap data if needed.
  if (cgiWifiAps.apData!=NULL)
   {
    for (n=0; n<cgiWifiAps.noAps; n++) os_free(cgiWifiAps.apData[n]);
    os_free(cgiWifiAps.apData);
   }
  // Count amount of access points found.
  n=0;
  while (bss_link != NULL)
   {
    bss_link = bss_link->next.stqe_next;
    n++;
   }
  // Allocate memory for access point data
  cgiWifiAps.apData=(ApData **)os_malloc(sizeof(ApData *)*n);
  cgiWifiAps.noAps=n;
  os_printf("Scan done: found %d APs\n", n);
  // Copy access point data to the static struct
  n=0;
  bss_link = (struct bss_info *)arg;
  while (bss_link != NULL)
   {
    if (n>=cgiWifiAps.noAps)
     {
      // This means the bss_link changed under our nose. Shouldn't happen!
      // Break because otherwise we will write in unallocated memory.
      os_printf("Huh? I have more than the allocated %d aps!\n", cgiWifiAps.noAps);
      break;
     }
    // Save the ap data.
    cgiWifiAps.apData[n]=(ApData *)os_malloc(sizeof(ApData));
    cgiWifiAps.apData[n]->rssi=bss_link->rssi;
    cgiWifiAps.apData[n]->enc=bss_link->authmode;
    strncpy(cgiWifiAps.apData[n]->ssid, (char*)bss_link->ssid, 32);
    bss_link = bss_link->next.stqe_next;
    n++;
   }
  // We're done.
  cgiWifiAps.scanInProgress=0;
 }


// Routine to start a WiFi access point scan.
static void ICACHE_FLASH_ATTR wifiStartScan()
 {
  if (cgiWifiAps.scanInProgress) return;
  cgiWifiAps.scanInProgress=1;
  wifi_station_scan(NULL, wifiScanDoneCb);
 }


// Initiate a WiFi scan or show the result of an earlier scan
int ICACHE_FLASH_ATTR cgiWiFiScan(HttpdConnData *connData)
 {
  int pos=(int)connData->cgiData;
  int len;
  char buff[1024];
  if (!cgiWifiAps.scanInProgress && pos!=0)
   {
    // Fill in json code for an access point
    if (pos-1<cgiWifiAps.noAps)
     {
      len=os_sprintf(buff, "  {\"essid\": \"%s\", \"rssi\": \"%d\", \"enc\": \"%d\"}%s\n", 
      cgiWifiAps.apData[pos-1]->ssid, cgiWifiAps.apData[pos-1]->rssi, cgiWifiAps.apData[pos-1]->enc, (pos-1==cgiWifiAps.noAps-1)?"":",");
      httpdSend(connData, buff, len);
     }
    pos++;
    if ((pos-1)>=cgiWifiAps.noAps)
     {
      len=os_sprintf(buff, " ]\n}\n");
      httpdSend(connData, buff, len);
      // Also start a new scan.
      wifiStartScan();
      return HTTPD_CGI_DONE;
     } else {
      connData->cgiData=(void*)pos;
      return HTTPD_CGI_MORE;
     }
   }
  httpdStartResponse(connData, 200);
  httpdHeader(connData, "Content-Type", "text/json");
  httpdHeader(connData, "Pragma", "no-cache");
  httpdHeader(connData, "Expires", "0");
  httpdEndHeaders(connData);
  if (cgiWifiAps.scanInProgress==1)
   {
    // We're still scanning.
    len=os_sprintf(buff, "{\n \"inProgress\": \"1\"\n}\n");
    httpdSend(connData, buff, len);
    return HTTPD_CGI_DONE;
   } else {
    // We have a scan result. Pass it on.
    len=os_sprintf(buff, "{\n \"inProgress\": \"0\",\n \"APs\": [\n");
    httpdSend(connData, buff, len);
    if (cgiWifiAps.apData==NULL) cgiWifiAps.noAps=0;
    connData->cgiData=(void *)1;
    return HTTPD_CGI_MORE;
   }
 }


// Load or save WiFi settings
int ICACHE_FLASH_ATTR cgiWiFiSettings(HttpdConnData *connData)
 {
  static struct station_config stconf;
  char buff[1024];
  char essid[32];
  char passwd[64];
  int len, x;
  essid[0]='\0';
  passwd[0]='\0';
  // Check if connection exists
  if (connData->conn==NULL)
   {
    return HTTPD_CGI_DONE;
   }
  // Get station config
  wifi_station_get_config(&stconf);
  // HTTP Header
  httpdStartResponse(connData, 200);
  httpdHeader(connData, "Content-Type", "text/json");
  httpdHeader(connData, "Pragma", "no-cache");
  httpdHeader(connData, "Expires", "0");
  httpdEndHeaders(connData);
  // Parse arguments
  len=httpdFindArg(connData->getArgs, "action", buff, sizeof(buff));
  if (len!=0)
   {
    if(os_strcmp(buff, "load")==0)
     {
      // Read settings
      os_strncpy(essid, (char*)stconf.ssid, sizeof(essid));
      #ifndef HIDE_PASS
      os_strncpy(passwd, (char*)stconf.password, sizeof(passwd));
      #else
      if(stconf.password[0]!='\0')
       {
        os_strncpy(passwd, HIDE_PASS_STRING, sizeof(passwd));
       } else {
        os_strcpy(passwd, "");
       }
      #endif
      x=wifi_get_opmode();
      len=os_sprintf(buff, "{\n \"wifi_mode\": \"%d\",\n \"wifi_essid\": \"%s\",\n \"wifi_pass\": \"%s\"\n}\n", x, essid, passwd);
      httpdSend(connData, buff, len);
     } else
      if(os_strcmp(buff, "save")==0)
       {
        // Save settings
        httpdFindArg(connData->post->buff, "wifi_essid", essid, sizeof(essid));
        essid[(sizeof(essid)-1)] = '\0';
        httpdFindArg(connData->post->buff, "wifi_pass", passwd, sizeof(passwd));
        passwd[(sizeof(passwd)-1)] = '\0';
        #ifdef HIDE_PASS
        // If password is equal to replacement string, password has not been changed
        // so we reload the old password before saving the configuration
        if(os_strcmp(passwd, HIDE_PASS_STRING)==0)
         {
          os_strcpy(passwd, (char*)stconf.password); // Load old PW
         }
        #endif
        // Write config
        os_strncpy((char*)stconf.ssid, essid, sizeof(stconf.ssid));
        os_strncpy((char*)stconf.password, passwd, sizeof(stconf.password));
        wifi_station_set_config(&stconf);
        len=os_sprintf(buff, "{\n \"status\": \"success\"\n}\n");
        httpdSend(connData, buff, len);
       } else {
        len=os_sprintf(buff, "{\n \"status\": \"error\"\n}\n");
        httpdSend(connData, buff, len);
       }
   } else {
    len=os_sprintf(buff, "{\n \"status\": \"error\"\n}\n");
    httpdSend(connData, buff, len);
   }
  return HTTPD_CGI_DONE;
 }
