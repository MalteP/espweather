// #############################################################################
// #                        ESP8266 WiFi Weather Sensor                        #
// #############################################################################
// # cgiconfig.h - CGI for "Sensor settings" and "User settings" tab           #
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

#ifndef CGICONFIG_H
 #define CGICONFIG_H

 #include "httpd.h"

 int cgiSensorConfig(HttpdConnData *connData);
 int cgiUserConfig(HttpdConnData *connData);

#endif
