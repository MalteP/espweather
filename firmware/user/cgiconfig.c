// #############################################################################
// #                        ESP8266 WiFi Weather Sensor                        #
// #############################################################################
// # cgiconfig.c - CGI for "Sensor settings" and "User settings" tab           #
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
#include "cgiconfig.h"
#include "config.h"


// Load and save sensor settings
int ICACHE_FLASH_ATTR cgiSensorConfig(HttpdConnData *connData)
 {
  char buff[1536];
  char tmpstr[8];
  int len;
  MyConfig *cfgdummy;
  char passtmp[sizeof(cfgdummy->mqtt_pass)];
  // Check if connection exists
  if(connData->conn==NULL)
   {
    return HTTPD_CGI_DONE;
   }
  // HTTP Header
  httpdStartResponse(connData, 200);
  httpdHeader(connData, "Content-Type", "text/json");
  httpdHeader(connData, "Pragma", "no-cache");
  httpdHeader(connData, "Expires", "0");
  httpdEndHeaders(connData);
  // Parse arguments
  len=httpdFindArg(connData->getArgs, "action", buff, sizeof(buff));
  if(len!=0)
   {
    if(os_strcmp(buff, "load")==0)
     {
      // Read settings
      #ifndef HIDE_PASS
      os_strncpy(passtmp, configGet()->mqtt_pass, sizeof(passtmp));
      #else
      if(configGet()->mqtt_pass[0]!='\0')
       {
        os_strncpy(passtmp, HIDE_PASS_STRING, sizeof(passtmp));
       } else {
        os_strcpy(passtmp, "");
       }
      #endif
      len=os_sprintf(buff, "{\n \"sensor_interval\": \"%d\",\n \"http_enabled\": \"%d\",\n \"http_key\": \"%s\",\n \"http_grp\": \"%s\",\n \"http_url\": \"%s\",\n \"mqtt_enabled\": \"%d\",\n \"mqtt_host\": \"%s\",\n \"mqtt_port\": \"%d\",\n \"mqtt_user\": \"%s\",\n \"mqtt_pass\": \"%s\",\n \"mqtt_topic\": \"%s\"\n}\n", configGet()->sensor_interval, configGet()->http_enabled, configGet()->http_key, configGet()->http_grp, configGet()->http_url, configGet()->mqtt_enabled, configGet()->mqtt_host, configGet()->mqtt_port, configGet()->mqtt_user, passtmp, configGet()->mqtt_topic);
      httpdSend(connData, buff, len);
     } else
      if(os_strcmp(buff, "save")==0)
       {
        // Save settings
        if(httpdFindArg(connData->post->buff, "sensor_interval", tmpstr, sizeof(tmpstr))!=0)
         {
          tmpstr[(sizeof(tmpstr)-1)] = '\0';
          configGet()->sensor_interval = atoi(tmpstr);
         }
        if(httpdFindArg(connData->post->buff, "http_enabled", tmpstr, sizeof(tmpstr))!=0)
         {
          tmpstr[(sizeof(tmpstr)-1)] = '\0';
          configGet()->http_enabled = atoi(tmpstr);
         }
        httpdFindArg(connData->post->buff, "http_key", configGet()->http_key, sizeof(cfgdummy->http_key));
        configGet()->http_key[(sizeof(cfgdummy->http_key)-1)] = '\0';
        httpdFindArg(connData->post->buff, "http_grp", configGet()->http_grp, sizeof(cfgdummy->http_grp));
        configGet()->http_grp[(sizeof(cfgdummy->http_grp)-1)] = '\0';
        httpdFindArg(connData->post->buff, "http_url", configGet()->http_url, sizeof(cfgdummy->http_url));
        configGet()->http_url[(sizeof(cfgdummy->http_url)-1)] = '\0';
        if(httpdFindArg(connData->post->buff, "mqtt_enabled", tmpstr, sizeof(tmpstr))!=0)
         {
          tmpstr[(sizeof(tmpstr)-1)] = '\0';
          configGet()->mqtt_enabled = atoi(tmpstr);
         }
        httpdFindArg(connData->post->buff, "mqtt_host", configGet()->mqtt_host, sizeof(cfgdummy->mqtt_host));
        configGet()->mqtt_host[(sizeof(cfgdummy->mqtt_host)-1)] = '\0';
        if(httpdFindArg(connData->post->buff, "mqtt_port", tmpstr, sizeof(tmpstr))!=0)
         {
          tmpstr[(sizeof(tmpstr)-1)] = '\0';
          configGet()->mqtt_port = atoi(tmpstr);
         }
        httpdFindArg(connData->post->buff, "mqtt_user", configGet()->mqtt_user, sizeof(cfgdummy->mqtt_user));
        configGet()->mqtt_user[(sizeof(cfgdummy->mqtt_user)-1)] = '\0';
        httpdFindArg(connData->post->buff, "mqtt_pass", passtmp, sizeof(passtmp));
        passtmp[(sizeof(passtmp)-1)] = '\0';
        #ifndef HIDE_PASS
        os_strncpy(configGet()->mqtt_pass, passtmp, sizeof(cfgdummy->mqtt_pass));
        #else
        // If password is not equal to replacement string, password has been changed
        if(os_strcmp(passtmp, HIDE_PASS_STRING)!=0)
         {
          os_strncpy(configGet()->mqtt_pass, passtmp, sizeof(cfgdummy->mqtt_pass));
         }
        #endif
        httpdFindArg(connData->post->buff, "mqtt_topic", configGet()->mqtt_topic, sizeof(cfgdummy->mqtt_topic));
        configGet()->mqtt_topic[(sizeof(cfgdummy->mqtt_topic)-1)] = '\0';
        configSave();
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


// Load and save user settings
int ICACHE_FLASH_ATTR cgiUserConfig(HttpdConnData *connData)
 {
  char buff[1536];
  int len;
  MyConfig *cfgdummy;
  char passtmp[sizeof(cfgdummy->mqtt_pass)];
  // Check if connection exists
  if(connData->conn==NULL)
   {
    return HTTPD_CGI_DONE;
   }
  // HTTP Header
  httpdStartResponse(connData, 200);
  httpdHeader(connData, "Content-Type", "text/json");
  httpdHeader(connData, "Pragma", "no-cache");
  httpdHeader(connData, "Expires", "0");
  httpdEndHeaders(connData);
  // Parse arguments
  len=httpdFindArg(connData->getArgs, "action", buff, sizeof(buff));
  if(len!=0)
   {
    if(os_strcmp(buff, "load")==0)
     {
      // Read settings
      #ifndef HIDE_PASS
      os_strncpy(passtmp, configGet()->admin_pass, sizeof(passtmp));
      #else
      if(configGet()->admin_pass[0]!='\0')
       {
        os_strncpy(passtmp, HIDE_PASS_STRING, sizeof(passtmp));
       } else {
        os_strcpy(passtmp, "");
       }
      #endif
      len=os_sprintf(buff, "{\n \"admin_user\": \"%s\",\n \"admin_pass\": \"%s\"\n}\n", configGet()->admin_user, passtmp);
      httpdSend(connData, buff, len);
     } else
      if(os_strcmp(buff, "save")==0)
       {
        // Save settings
        httpdFindArg(connData->post->buff, "admin_user", configGet()->admin_user, sizeof(cfgdummy->admin_user));
        configGet()->admin_user[(sizeof(cfgdummy->admin_user)-1)] = '\0';
        httpdFindArg(connData->post->buff, "admin_pass", passtmp, sizeof(passtmp));
        passtmp[(sizeof(passtmp)-1)] = '\0';
        #ifndef HIDE_PASS
        os_strncpy(configGet()->admin_pass, passtmp, sizeof(cfgdummy->admin_pass));
        #else
        // If password is not equal to replacement string, password has been changed
        if(os_strcmp(passtmp, HIDE_PASS_STRING)!=0)
         {
          os_strncpy(configGet()->admin_pass, passtmp, sizeof(cfgdummy->admin_pass));
         }
        #endif
        configSave();
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
