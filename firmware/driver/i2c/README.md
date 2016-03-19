esp8266 i2c driver

This driver is far from perfect and still needs work, if you have any ideas or questions please create a issue.

```
cd driver/
git clone https://github.com/zarya/esp8266_i2c_driver.git i2c
```
* Add ```#include "i2c/i2c.h"``` to your code
* Add ```driver/i2c``` to the MODULES line in the makefile
