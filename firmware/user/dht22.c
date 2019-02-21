// #############################################################################
// #                        ESP8266 WiFi Weather Sensor                        #
// #############################################################################
// # dht22.c - Functions for DHT22 temperature / humidity sensor               #
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
#include "dht22.h"

int dht22delay = 0;

void dht22TimerCb( void *arg );


// Init sensor
int ICACHE_FLASH_ATTR dht22Init( struct dhtdata* d )
 {
  static ETSTimer dht22Timer;
  // GPIO2: DHT22 data
  PIN_FUNC_SELECT(DHT22_MUX, DHT22_FUNC);
  GPIO_DIS_OUTPUT(DHT22_GPIO);
  d->humidity = 0;
  d->temperature = 0;
  // Because GPIO2 (DHT22 pin) is alternative UART TX in bootloader mode, the sensor seems
  // to get a little bit confused and needs some delay before first read after boot...
  dht22delay = 1;
  os_timer_disarm(&dht22Timer);
  os_timer_setfn(&dht22Timer, dht22TimerCb, NULL);
  os_timer_arm(&dht22Timer, DHT22_READ_DELAY_MS, 0);
  return 0;
 }


// First read a few hundred milliseconds delayed
void ICACHE_FLASH_ATTR dht22TimerCb( void *arg )
 {
  dht22delay = 0;
 }


// Read values from sensor
int ICACHE_FLASH_ATTR dht22Read( struct dhtdata* d )
 {
  unsigned int i;
  unsigned int bitctr = 0;
  uint8_t chksum = 0;

  // Initial read delay
  if(dht22delay>0) return -1;

  // Reset data
  for(i=0; i<5; i++)
   {
    d->rawdata[i] = 0;
   }
  d->humidity = 0;
  d->temperature = 0;

  // Disable interrupts and reset watchdog
  ets_intr_lock();
  system_soft_wdt_feed();

  // Start signal (1ms low pulse)
  GPIO_OUTPUT_SET(DHT22_GPIO, 0);
  os_delay_us(DHT22_T_START_US);

  // Switch pin to input
  GPIO_DIS_OUTPUT(DHT22_GPIO);
  // Bus master release
  os_delay_us(DHT22_T_RELEASE_US);

  // Now the sensor will pull the bus low and high
  i = DHT22_T_RESPONSE_US;
  while(GPIO_INPUT_GET(DHT22_GPIO) == 0)
   {
    os_delay_us(1);
    if(--i==0) goto errorfunction; // Timeout
   }
  i = DHT22_T_RESPONSE_US;
  while(GPIO_INPUT_GET(DHT22_GPIO) == 1)
   {
    os_delay_us(1);
    if(--i==0) goto errorfunction; // Timeout
   }

  // Read 40 bits of data
  for(bitctr=0; bitctr<40; bitctr++)
   {
    // Bit 0 & 1 low pulse
    i = DHT22_T_LOW;
    while(GPIO_INPUT_GET(DHT22_GPIO) == 0)
     {
      os_delay_us(1);
      if(--i==0) goto errorfunction; // Timeout
     }
    // High pulse, measure time
    i = 0;
    while(GPIO_INPUT_GET(DHT22_GPIO) == 1)
     {
      os_delay_us(1);
      if(++i>DHT22_T_HIGH_1) goto errorfunction; // Timeout
     }
    d->rawdata[bitctr/8] <<= 1;
    if(i>DHT22_T_HIGH_0) // This is no "0" ;-)
     {
      d->rawdata[bitctr/8] |= 1;
     }
   }

  // Enable interrupts
  ets_intr_unlock();

  // Print values
  //os_printf("DHT22 read: %02x %02x %02x %02x %02x\n", d->rawdata[0], d->rawdata[1], d->rawdata[2], d->rawdata[3], d->rawdata[4]);

  // Calculate checksum
  for(i=0; i<4; i++)
   {
    chksum += d->rawdata[i];
   }
  if(d->rawdata[4]!=chksum)
   {
    //os_printf("DHT22 checksum mismatch: %02x\n", chksum);
    goto endfunction;
   }

  d->humidity = ((uint16_t)d->rawdata[0]<<8) + d->rawdata[1];
  d->temperature = (int16_t)(((uint16_t)d->rawdata[2]<<8) + d->rawdata[3]);
  os_printf("DHT22: t=%d, h=%d\n", d->temperature, d->humidity);

  goto endfunction;

  errorfunction:
  // Enable interrupts
  ets_intr_unlock();

  // Print errormessage
  os_printf("DHT22 read failed. %d bits received.\n", bitctr);
  return -1;

  endfunction:
  return 0;
 }
