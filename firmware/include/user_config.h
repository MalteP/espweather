#ifndef _USER_CONFIG_H_
#define _USER_CONFIG_H_

#define MQTT_BUF_SIZE		1024
#define MQTT_RECONNECT_TIMEOUT 	5	/*second*/

#define PROTOCOL_NAMEv31	/*MQTT version 3.1 compatible with Mosquitto v0.15*/
//PROTOCOL_NAMEv311			/*MQTT version 3.11 compatible with https://eclipse.org/paho/clients/testing/*/

#ifdef ENABLE_SSL
#define MQTT_SSL_ENABLE
#define HTTPCLIENT_SECURE
#endif

#endif
