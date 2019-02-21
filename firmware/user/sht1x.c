// #############################################################################
// #                        ESP8266 WiFi Weather Sensor                        #
// #############################################################################
// # sht1x.c - Functions for SHT11/12/15 temperature / humidity sensor         #
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
#include "sht1x.h" 


// Initialize sensor
int ICACHE_FLASH_ATTR shtInit( struct shtdata* d )
 {
  int rtn = -1;
  uint16_t status;
  uint8_t crc_calc;

  // Disable GPIO interrupts
  ETS_INTR_LOCK();

  // Set pin function to GPIO
  PIN_FUNC_SELECT(SHT_DATA_MUX, SHT_DATA_FUNC);
  PIN_FUNC_SELECT(SHT_CLK_MUX,  SHT_CLK_FUNC);

  // Data as open drain
  GPIO_REG_WRITE(GPIO_PIN_ADDR(GPIO_ID_PIN(SHT_DATA_PIN)), GPIO_REG_READ(GPIO_PIN_ADDR(GPIO_ID_PIN(SHT_DATA_PIN))) | GPIO_PIN_PAD_DRIVER_SET(GPIO_PAD_DRIVER_ENABLE));
  GPIO_REG_WRITE(GPIO_ENABLE_ADDRESS, GPIO_REG_READ(GPIO_ENABLE_ADDRESS) | (1 << SHT_DATA_PIN));

  // Clock as open drain
  GPIO_REG_WRITE(GPIO_PIN_ADDR(GPIO_ID_PIN(SHT_CLK_PIN)), GPIO_REG_READ(GPIO_PIN_ADDR(GPIO_ID_PIN(SHT_CLK_PIN))) | GPIO_PIN_PAD_DRIVER_SET(GPIO_PAD_DRIVER_ENABLE));
  GPIO_REG_WRITE(GPIO_ENABLE_ADDRESS, GPIO_REG_READ(GPIO_ENABLE_ADDRESS) | (1 << SHT_CLK_PIN));

  // Enable GPIO interrupts again
  ETS_INTR_UNLOCK();

  // Set ports
  SHT_DATA_HIGH();
  SHT_CLK_LOW();

  // Reset data
  d->temperature = 0;
  d->humidity = 0;

  // Reset sensor
  shtReset();

  // Check for sensor
  shtStart();
  shtWrite8(SHT_CMD_READ_STATUS_REG);
  if(shtReadAck()!=0) goto endfunction;
  shtRead16();
  shtSendAck();
  rtn = 0;

  endfunction:

  // Reduce power consumption through pullup - same port as i2c
  SHT_CLK_HIGH();

  return rtn;
 }


// Read temperature & humidity
int ICACHE_FLASH_ATTR shtRead( struct shtdata* d )
 {
  int rtn = -1;
  uint16_t t, h;
  uint8_t temp;
  #ifdef SHT_USE_CRC
  uint8_t crc, crc_calc;
  #endif
  // Read temperature
  shtStart();
  shtWrite8(SHT_CMD_MEASURE_TEMPERATURE);
  if(shtReadAck()!=0) goto endfunction;
  temp=shtWaitForConversion(SHT_MEASUREMENT14_MS);
  if(temp!=0) goto endfunction;
  t = shtRead16();
  #ifndef SHT_USE_CRC
  shtSkipAck();
  #else
  shtSendAck();
  crc = shtRead8();
  shtSendAck();
  crc_calc = shtCalculateCRC(0x00, SHT_CMD_MEASURE_TEMPERATURE, (uint8_t)(t>>8), (uint8_t)t);
  if(crc!=crc_calc)
   {
    os_printf("SHT1x: checksum mismatch\n");
    goto endfunction;
   }
  #endif
  // Calculate temperature
  d->temperature = shtConvertTemperature(t);
  // Read humidity
  shtStart();
  shtWrite8(SHT_CMD_MEASURE_HUMIDITY);
  if(shtReadAck()!=0) goto endfunction;
  temp=shtWaitForConversion(SHT_MEASUREMENT12_MS);
  if(temp!=0) goto endfunction;
  h = shtRead16();
  #ifndef SHT_USE_CRC
  shtSkipAck();
  #else
  shtSendAck();
  crc = shtRead8();
  shtSendAck();
  crc_calc = shtCalculateCRC(0x00, SHT_CMD_MEASURE_HUMIDITY, (uint8_t)(h>>8), (uint8_t)h);
  if(crc!=crc_calc)
   {
    os_printf("SHT1x: checksum mismatch\n");
    goto endfunction;
   }
  #endif
  // Calculate compensated humidity
  d->humidity = shtConvertHumidity(h, d->temperature);
  // Done.
  //os_printf("SHT1x tRaw=%d, hRaw=%d", t, h);
  os_printf("SHT1x: t=%d, h=%d\n", d->temperature, d->humidity);
  rtn=0;
  endfunction:
  // Reset port
  SHT_CLK_HIGH();
  SHT_DATA_HIGH();
  return rtn;
 }


// Bus reset
void ICACHE_FLASH_ATTR shtReset( void )
 {
  uint8_t i;
  SHT_DATA_HIGH();
  SHT_CLK_LOW();
  shtDelay();
  // Pulse 9 times to reset sensor interface
  for(i=9; i>0; i--)
   {
    SHT_CLK_HIGH();
    shtDelay();
    SHT_CLK_LOW();
    shtDelay();
   }
 }


// Start condition
void ICACHE_FLASH_ATTR shtStart( void )
 {
  SHT_DATA_HIGH();
  SHT_CLK_LOW();
  shtDelay();
  SHT_CLK_HIGH();
  shtDelay();
  SHT_DATA_LOW();
  shtDelay();
  SHT_CLK_LOW();
  shtDelay();
  SHT_CLK_HIGH();
  shtDelay();
  SHT_DATA_HIGH();
  shtDelay();
  SHT_CLK_LOW();
  shtDelay();
 }


// Ack
void ICACHE_FLASH_ATTR shtSendAck( void )
 {
  SHT_DATA_LOW();
  SHT_CLK_HIGH();
  shtDelay();
  SHT_CLK_LOW();
  shtDelay();
  SHT_DATA_HIGH();
 }


// Skip ack
void ICACHE_FLASH_ATTR shtSkipAck( void )
 {
  SHT_DATA_HIGH();
  SHT_CLK_HIGH();
  shtDelay();
  SHT_CLK_LOW();
  shtDelay();
 }


// Wait for ack from sensor
uint8_t ICACHE_FLASH_ATTR shtReadAck( void )
 {
  uint8_t i = SHT_DATA_VALID_NS;
  SHT_DATA_HIGH();
  // Wait for low pulse
  while(SHT_READ() != 0)
   {
    if(--i<=0) return 1; // Timeout
   } 
  SHT_CLK_HIGH();
  shtDelay();
  SHT_CLK_LOW();
  shtDelay();
  return 0;
 }


// Read 8bit value
uint8_t ICACHE_FLASH_ATTR shtRead8( void )
 {
  uint8_t i;
  uint8_t data = 0;
  SHT_DATA_HIGH();
  SHT_CLK_LOW();
  for(i=8; i>0; i--)
   {
    SHT_CLK_HIGH();
    shtDelay();
    data<<=1;
    data |= SHT_READ();
    SHT_CLK_LOW();
    shtDelay();
   }
  return data;
 }


// Read 16bit value
uint16_t ICACHE_FLASH_ATTR shtRead16( void )
 {
  uint16_t data = 0;
  data = shtRead8();
  data<<=8;
  shtSendAck();
  data |= shtRead8();
  return data;
 }


// Write 8bit value
void ICACHE_FLASH_ATTR shtWrite8( uint8_t data )
 {
  uint8_t i;
  SHT_DATA_HIGH();
  SHT_CLK_LOW();
  for(i=8; i>0; i--)
   {
    if(data&0x80)
     {
      SHT_DATA_HIGH();
     } else {
      SHT_DATA_LOW();
     }
    SHT_CLK_HIGH();
    shtDelay();
    SHT_CLK_LOW();
    shtDelay();
    data<<=1;
   }
  SHT_DATA_HIGH();
 }


// Wait until sensor finishes ADC conversion
uint8_t ICACHE_FLASH_ATTR shtWaitForConversion( uint16_t time )
 {
  SHT_DATA_HIGH();
  SHT_CLK_LOW();
  while(SHT_READ() != 0)
   {
    system_soft_wdt_feed();
    os_delay_us(1000);
    if(--time<=0) return 1; // Timeout
   }
  return 0;
 }


// Delay between pulses
void ICACHE_FLASH_ATTR shtDelay( void )
 {
  os_delay_us(SHT_TSCK_NS);
 }


// Will return temperature x10
#ifdef USE_FLOAT_CALC
int16_t ICACHE_FLASH_ATTR shtConvertTemperature( uint16_t temperature_raw )
 {
  float temperature;
  temperature = SHT_TEMPERATURE_D1 + SHT_TEMPERATURE_D2 * (float)temperature_raw;
  return (uint16_t)(temperature*10);
 }
#else
int16_t ICACHE_FLASH_ATTR shtConvertTemperature( uint16_t temperature_raw )
 {
  int16_t temperature;
  temperature = SHT_TEMPERATURE_D1*100;
  temperature += SHT_TEMPERATURE_D2*100 * temperature_raw;
  temperature = temperature/10; // (/100 *10)
  return temperature;
 }
#endif


// Will return humidity x10
#ifdef USE_FLOAT_CALC
int16_t ICACHE_FLASH_ATTR shtConvertHumidity( uint16_t humidity_raw, int16_t temperature )
 {
  int16_t humidity_compensated = 0;
  float linear, humidity;
  linear = SHT_HUMIDITY_C1;
  linear += SHT_HUMIDITY_C2 * humidity_raw;
  linear += SHT_HUMIDITY_C3 * humidity_raw * humidity_raw;
  humidity = (temperature-25*10)*(SHT_HUMIDITY_T1 + SHT_HUMIDITY_T2 * (float)humidity_raw);
  humidity_compensated += (int16_t)humidity + (int16_t)(linear*10);
  if(humidity_compensated>1000) humidity_compensated = 1000;
  if(humidity_compensated<0) humidity_compensated = 0;
  return humidity_compensated;
 }
#else
int16_t ICACHE_FLASH_ATTR shtConvertHumidity( uint16_t humidity_raw, int16_t temperature )
 {
  int32_t humidity;
  int32_t linear;
  int32_t linear2;
  // Calculate linear part x10
  linear = SHT_HUMIDITY_C1*10000;
  linear += SHT_HUMIDITY_C2*10000 * humidity_raw;
  linear2 = SHT_HUMIDITY_C3*10000000 * humidity_raw*humidity_raw;
  linear += linear2 / 1000;
  linear = linear / 1000;
  // Calculate humidity x10
  humidity = SHT_HUMIDITY_T1*10000;
  humidity += SHT_HUMIDITY_T2*10000 * humidity_raw;
  humidity = humidity/10 * (temperature-250);
  humidity = humidity / 1000;
  // Final calculation
  humidity += linear;
  if(humidity>1000) humidity = 1000;
  if(humidity<0) humidity = 0;
  return humidity;
 }
#endif


#ifdef SHT_USE_CRC
// Used for CRC check
uint8_t ICACHE_FLASH_ATTR shtReverseByte( uint8_t a )
 {
  uint8_t i, b = 0;
  for(i=8; i>0; i--)
   {
    b<<=1;
    b |= (a&0x01);
    a>>=1;
   }
  return b;
 }


// Check CRC of sensor data
uint8_t ICACHE_FLASH_ATTR shtCalculateCRC( uint8_t start, uint8_t byte0, uint8_t byte1, uint8_t byte2 )
 {
  // Start value: data of status register (reversed!)
  uint8_t crc = shtReverseByte(start);
  // Now every value is XOR'd with the previous CRC value
  crc = sht_crc_table[byte0^crc];
  crc = sht_crc_table[byte1^crc];
  crc = sht_crc_table[byte2^crc];
  // Reverse value again
  return shtReverseByte(crc);
 }
#endif
