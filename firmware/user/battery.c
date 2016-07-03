// #############################################################################
// #                        ESP8266 WiFi Weather Sensor                        #
// #############################################################################
// # battery.c - Battery voltage measurement                                   #
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

#include <esp8266.h>
#include "battery.h"

unsigned int battery_voltage; 


// Initialize port
void ICACHE_FLASH_ATTR batteryInit( void )
 {
  PIN_FUNC_SELECT(BATT_FET_MUX, BATT_FET_FUNC);
  gpio_output_set((1<<BATT_FET_PIN), 0, (1<<BATT_FET_PIN), 0);
  battery_voltage = 0;
 }


// Measure battery voltage
void ICACHE_FLASH_ATTR batteryMeasureVoltage( void )
 {
  unsigned int adc;
  // 1V = 1024, Voltage divider: 5,6K / 1K
  // Voltage reference in ESP8266 is 1,00V -> 6,6V in = 1024
  // Calculation in millivolts (1V -> 1000mV)
  // GPIO13 is triggered to enable voltage measurement FET
  gpio_output_set(0, (1<<BATT_FET_PIN), (1<<BATT_FET_PIN), 0);
  adc = system_adc_read();
  battery_voltage = (adc*1000*6.6)/1024;
  gpio_output_set((1<<BATT_FET_PIN), 0, (1<<BATT_FET_PIN), 0);
  battery_voltage += BATTERY_VOLTAGE_OFFSET_MV;
  os_printf("Battery: ADC=%d, BatMv=%d\n", adc, battery_voltage);
 }


// Return measured voltage
unsigned int ICACHE_FLASH_ATTR batteryGetVoltage( void )
 {
  return battery_voltage;
 }


// Check if voltage is below limit
int ICACHE_FLASH_ATTR batteryCheckVoltage( void )
 {
  if(battery_voltage<BATTERY_VOLTAGE_LIMIT_MV)
   {
    os_printf("Battery: Voltage below %d mV limit!\n", BATTERY_VOLTAGE_LIMIT_MV);
    return 0;
   }
  return 1;
 }
