// #############################################################################
// #                        ESP8266 WiFi Weather Sensor                        #
// #############################################################################
// # config.h - Load or save configuration in flash, config button handler     #
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

#ifndef CONFIG_H
 #define CONFIG_H

 // GPIO pin for config button
 #define BTNGPIO 0

 // GPIO for status LED
 #define LEDGPIO 14

 // Flash position for config data
 #define ESP_PARAM_SEC 0x7B

 // RTC magic value for detecting first start
 #define RTC_MAGIC 0xC0DEBABE

 // Sensor modes
 #define MODE_NORMAL 0
 #define MODE_CONFIG 1
 #define MODE_DEFAULT MODE_NORMAL

 // Hide all passwords in setup?
 #define HIDE_PASS
 #define HIDE_PASS_STRING "******"

 // Default user for login
 #define ADMIN_USER "admin"
 #define ADMIN_PASS "insecure"

 // Push interval & initial settings
 #define INT_DEFAULT 300
 #define HTTP_ENABLED 0
 #define MQTT_ENABLED 0

 // Default data for cloud service

 #define URL_DEFAULT "http://example.com/sensor.php?temperature=%t&humidity=%h&pressure=%p&voltage=%v"
 #define KEY_DEFAULT ""
 #define GRP_DEFAULT ""

 // Default data for MQTT
 #define MQTT_HOST   "example.com"
 #define MQTT_PORT   1883 
 #define MQTT_USER   ""
 #define MQTT_PASS   ""
 #define MQTT_TOPIC  "/sensors/0"

 // Default data IP configuration
 #define WIFI_DHCP   1
 #define WIFI_IP     "192.168.0.99"
 #define WIFI_MASK   "255.255.255.0"
 #define WIFI_GW     "192.168.0.1"

 // LED status
 #define LED_OFF     0
 #define LED_FLASH1  1
 #define LED_FLASH2  2
 #define LED_FLASH3  3
 #define LED_FLASH4  4
 #define LED_BLINK   5

 // Configuration structure stored in flash
 // (Length must be multiple of 4 bytes because of SPI flash page structure)
 typedef struct
  {
   char chksum;
   uint8_t sensor_mode;
   uint16_t sensor_interval;
   char admin_user[32];
   char admin_pass[32];
   uint8_t http_enabled;
   char http_key[64];
   char http_grp[64];
   char http_url[128];
   uint8_t mqtt_enabled;
   char mqtt_host[64];
   uint16_t mqtt_port;
   char mqtt_user[64];
   char mqtt_pass[64];
   char mqtt_topic[64];
   uint8_t wifi_dhcp;
   char wifi_ip[16];
   char wifi_mask[16];
   char wifi_gw[16];
   char pad[3];
  } MyConfig;

 void configInit( void );
 void configLoad( void );
 void configSave( void );
 void configReset( void );
 MyConfig* configGet( void );
 uint8_t configIsFirstStart( void );
 void configCheckWifiMode( void );
 void configSetWifiMode( int mode );
 void configRestart( void );
 void configInitIP( void );
 void statusLed( uint8_t mode );

#endif
