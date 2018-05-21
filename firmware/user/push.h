// #############################################################################
// #                        ESP8266 WiFi Weather Sensor                        #
// #############################################################################
// # push.h - Push data to HTTP service or MQTT server                         #
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

#ifndef PUSH_H
 #define PUSH_H

 #include "mqtt.h"

 #define PUSH_CHECK_INTERVAL_MSEC 500
 #define PUSH_ERROR_DELAY         5
 #define PUSH_TIMEOUT_SEC         45
 #define PUSH_RETRIES_MAX         5
 #define PUSH_REDIRECT_MAX        5

 enum push_states { PUSH_IDLE, PUSH_INIT, PUSH_READ, PUSH_IPWAIT, PUSH_HTTPSTART, PUSH_HTTP, PUSH_HTTPWAIT, PUSH_MQTTSTART, PUSH_MQTT, PUSH_MQTTWAIT, PUSH_FINISH };

 #define HTTP_THINGSPEAK "http://api.thingspeak.com/update?api_key=%s&field1=%s&field2=%s&field3=%s&field4=%s&field5=%s"
 #define HTTP_ADAFRUIT   "http://io.adafruit.com/api/groups/%s/send.json?x-aio-key=%s&temperature=%s&humidity=%s&pressure=%s&voltage=%s&rssi=%s"
 #define HTTP_SPARKFUN   "http://data.sparkfun.com/input/%s?private_key=%s&temperature=%s&humidity=%s&pressure=%s&voltage=%s&rssi=%s"

 void pushSensorData( void );
 void pushTimer( void );
 void pushTimerCb( void *arg );
 uint8_t httpPush( void );
 void httpPushCb( char *response, int http_status, char *full_response );
 uint8_t mqttPush( void );
 void mqttPushCb(uint32_t *args);
 void mqttPublish(MQTT_Client* client, char* topic, char* sub, char* msg);
 void mqttPublishCb(uint32_t *args);
 void mqttErrorCb(uint32_t *args);
 void pushTimeout( void );
 static void pushTimeoutCb( void *arg );
 void sleepmode( void );

#endif
