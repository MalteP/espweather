// #############################################################################
// #                        ESP8266 WiFi Weather Sensor                        #
// #############################################################################
// # config.c - Load or save configuration in flash, config button handler     #
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

static MyConfig myConfig;
static ETSTimer configBtnTimer;
uint8_t firstStart;
uint8_t ledStatus, ledLastState;
uint8_t ledMode;

char calcChksum( void );
void configBtnTimerCb( void *arg);
void configRestartCb( void *arg );
void statusLedCb( void );


// Initialize configuration
void ICACHE_FLASH_ATTR configInit( void )
 {
  uint32_t rtcmagic = 0;
  // Clear config data
  memset(&myConfig, 0, sizeof(MyConfig));
  // Check for first start
  // This is done by storing a magic number inside the RTC register, which will
  // store its values during low power standby as long as the battery is connected.
  firstStart = 0;
  system_rtc_mem_read(128, &rtcmagic, 4);
  if(rtcmagic!=RTC_MAGIC)
   {
    rtcmagic=RTC_MAGIC;
    system_rtc_mem_write(128, &rtcmagic, 4);
    firstStart = 1;
   }
  // Init button
  PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0);
  os_timer_disarm(&configBtnTimer);
  os_timer_setfn(&configBtnTimer, configBtnTimerCb, NULL);
  os_timer_arm(&configBtnTimer, 500, 1);
  // Init LED
  ledStatus = 0;
  ledLastState = 0;
  ledMode = LED_OFF;
  PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, FUNC_GPIO14);
  GPIO_OUTPUT_SET(LEDGPIO, 0);
 }


// Load config from flash
void ICACHE_FLASH_ATTR configLoad( void )
 {
  int c;
  spi_flash_read(ESP_PARAM_SEC*SPI_FLASH_SEC_SIZE, (uint32 *)&myConfig, sizeof(MyConfig));
  c=calcChksum();
  if (c!=myConfig.chksum)
   {
    os_printf("Checksum mismatch. Loading default config!\n");
    configReset();
   }
 }


// Save config to flash
void ICACHE_FLASH_ATTR configSave( void )
 {
  myConfig.chksum=calcChksum();
  spi_flash_erase_sector(ESP_PARAM_SEC);
  spi_flash_write(ESP_PARAM_SEC*SPI_FLASH_SEC_SIZE, (uint32 *)&myConfig, sizeof(MyConfig));
 }


// Reset configuration to defaults
void ICACHE_FLASH_ATTR configReset( void )
 {
  struct station_config stconf;
  // Reset user config
  memset(&myConfig, 0, sizeof(MyConfig));
  myConfig.sensor_mode = MODE_DEFAULT;
  myConfig.sensor_interval = INT_DEFAULT;
  os_strcpy(myConfig.admin_user, ADMIN_USER);
  os_strcpy(myConfig.admin_pass, ADMIN_PASS);
  myConfig.http_enabled = HTTP_ENABLED;
  os_strcpy(myConfig.http_key, KEY_DEFAULT);
  os_strcpy(myConfig.http_grp, GRP_DEFAULT);
  os_strcpy(myConfig.http_url, URL_DEFAULT);
  myConfig.mqtt_enabled = MQTT_ENABLED;
  os_strcpy(myConfig.mqtt_host, MQTT_HOST);
  myConfig.mqtt_port = MQTT_PORT;
  os_strcpy(myConfig.mqtt_user, MQTT_USER);
  os_strcpy(myConfig.mqtt_pass, MQTT_PASS);
  os_strcpy(myConfig.mqtt_topic, MQTT_TOPIC);
  // Reset WLAN settings
  myConfig.wifi_dhcp = WIFI_DHCP;
  os_strcpy(myConfig.wifi_ip, WIFI_IP);
  os_strcpy(myConfig.wifi_mask, WIFI_MASK);
  os_strcpy(myConfig.wifi_gw, WIFI_GW);
  memset(&stconf, 0, sizeof(stconf));
  wifi_station_set_config(&stconf);
 }


// Return pointer to configuration
MyConfig* ICACHE_FLASH_ATTR configGet( void )
 {
  return &myConfig;
 }


// First start after battery was connected?
uint8_t ICACHE_FLASH_ATTR configIsFirstStart( void )
 {
  return firstStart;
 }


// Check and modify WiFi mode if necessary
void ICACHE_FLASH_ATTR configCheckWifiMode( void )
 {
  int mode = wifi_get_opmode();
  if(myConfig.sensor_mode==MODE_NORMAL)
   {
    if(mode!=1)
     {
      configSetWifiMode(1); // STA
      wifi_set_sleep_type(MODEM_SLEEP_T); // Save some power
     }
   } else {
    if(mode!=3)
     {
      configSetWifiMode(3); // AP+STA
      wifi_set_sleep_type(NONE_SLEEP_T); // No sleep possible
     }
   }
  // Only attempt to reconnect in STA mode, scanning might break AP mode
  if(mode==1)
   {
    wifi_station_set_reconnect_policy(true);
   } else {
    wifi_station_set_reconnect_policy(false);
   }
 }


// Set WiFi mode and restart
void ICACHE_FLASH_ATTR configSetWifiMode( int mode )
 {
  os_printf("Setting WIFI Mode to %d...\n", mode);
  wifi_station_disconnect();
  wifi_set_opmode(mode);
  configRestart();
 }


// Delayed restart
void ICACHE_FLASH_ATTR configRestart( void )
 {
  static ETSTimer configWifiTimer;
  GPIO_OUTPUT_SET(LEDGPIO, 0);
  os_timer_disarm(&configBtnTimer);
  // Delay restart
  os_timer_disarm(&configWifiTimer);
  os_timer_setfn(&configWifiTimer, configRestartCb, NULL);
  os_timer_arm(&configWifiTimer, 1000, 0);
}


// Callback for restart timer
void ICACHE_FLASH_ATTR configRestartCb( void *arg )
 {
  system_restart();
 }


// WiFi IP address configuration
void ICACHE_FLASH_ATTR configInitIP( void )
 {
  struct ip_info info;
  os_memset(&info, 0x0, sizeof(info));
  // DHCP configuration disabled?
  if(myConfig.wifi_dhcp==0)
   {
    wifi_station_dhcpc_stop();
    info.ip.addr = ipaddr_addr(myConfig.wifi_ip);
    info.netmask.addr = ipaddr_addr(myConfig.wifi_mask);
    info.gw.addr = ipaddr_addr(myConfig.wifi_gw);
    wifi_set_ip_info(STATION_IF, &info);
   }
 }


// SoftAP configuration
void ICACHE_FLASH_ATTR configInitSoftAP( void )
 {
  struct softap_config config;
  wifi_softap_get_config_default(&config);
  os_memset(config.password, 0, sizeof(config.password));
  #ifndef WIFI_PASS
  config.authmode = AUTH_OPEN;
  #else
  os_strncpy((char*)config.password, WIFI_PASS, sizeof(config.password)-1);
  config.authmode = AUTH_WPA_WPA2_PSK;
  #endif
  wifi_softap_set_config_current(&config);
 }


// Calculate config checksum
char ICACHE_FLASH_ATTR calcChksum( void )
 {
  char *p=(char*)&myConfig;
  int x;
  char r=0xa5;
  for (x=1; x<sizeof(MyConfig); x++) r+=p[x];
  return r;
 }


// Timer callback to poll config key
void ICACHE_FLASH_ATTR configBtnTimerCb( void *arg )
 {
  static int resetCnt=0;
  if(!GPIO_INPUT_GET(BTNGPIO))
   {
    resetCnt++;
   } else {
    // 10 sec pressed in config mode: Reset config
    if (resetCnt>=20&&myConfig.sensor_mode==MODE_CONFIG)
     {
      os_printf("Config: Reset\n");
      configReset();
      myConfig.sensor_mode=MODE_CONFIG;
      configSave();
      configRestart();
     } else {
      // 3 sec pressed: Switch sensor mode (normal / config)
      if (resetCnt>=6)
       {
        os_printf("Config: Switch mode\n");
        // If we are in normal mode, set to config mode and reset.
        if(myConfig.sensor_mode==MODE_NORMAL)
         {
          myConfig.sensor_mode=MODE_CONFIG;
         } else {
          myConfig.sensor_mode=MODE_NORMAL;
         }
        configSave();
        configCheckWifiMode();
       }
     }
    resetCnt=0;
   }
  statusLedCb();
 }


// Status LED callback
void ICACHE_FLASH_ATTR statusLedCb( void )
 {
  // LED off?
  if(ledMode==LED_OFF)
   {
    GPIO_OUTPUT_SET(LEDGPIO, 0);
    return;
   }
  if(ledMode<LED_BLINK)
   {
    // Flash x times
    if(ledStatus<ledMode)
     {
      GPIO_OUTPUT_SET(LEDGPIO, 1);
      os_delay_us(750);
      GPIO_OUTPUT_SET(LEDGPIO, 0);
     }
    if(++ledStatus>LED_FLASH4)
     {
      ledStatus = 0;
     }
   } else {
    // Blink
    ledStatus = 0;
    ledLastState = !ledLastState;
    GPIO_OUTPUT_SET(LEDGPIO, ledLastState);
   }
 }


// Set status led mode
void ICACHE_FLASH_ATTR statusLed( uint8_t mode )
 {
  if(mode!=ledMode) ledStatus=0xff; // Wait 500ms
  ledMode = mode;
 }
