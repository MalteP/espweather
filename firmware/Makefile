# --------------- esphttpd config options ---------------

# If GZIP_COMPRESSION is set to "yes" then the static css, js, and html files will be compressed with gzip before added to the espfs image
# and will be served with gzip Content-Encoding header.
# This could speed up the downloading of these files, but might break compatibility with older web browsers not supporting gzip encoding
# because Accept-Encoding is simply ignored. Enable this option if you have large static files to serve (for e.g. JQuery, Twitter bootstrap)
# By default only js, css and html files are compressed.
# If you have text based static files with different extensions what you want to serve compressed then you will need to add the extension to the following places:
# - Add the extension to this Makefile at the webpages.espfs target to the find command
# - Add the extension to the gzippedFileTypes array in the user/httpd.c file
#
# Adding JPG or PNG files (and any other compressed formats) is not recommended, because GZIP compression does not works effectively on compressed files.

# Static gzipping is disabled by default.
GZIP_COMPRESSION ?= no

# If COMPRESS_W_YUI is set to "yes" then the static css and js files will be compressed with yui-compressor
# This option works only when GZIP_COMPRESSION is set to "yes"
# http://yui.github.io/yuicompressor/
# Disabled by default.
COMPRESS_W_YUI ?= yes
YUI-COMPRESSOR ?= /usr/bin/yui-compressor

# If USE_HEATSHRINK is set to "yes" then the espfs files will be compressed with Heatshrink and decompressed
# on the fly while reading the file. Because the decompression is done in the esp8266, it does not require
# any support in the browser.
USE_HEATSHRINK ?= yes

# Enable SSL Support for HTTP and MQTT client
ENABLE_SSL ?= yes

# Flash size of ESP module in MBit (4m: 512KB / 8m: 1MB)
FLASH_SIZE ?= 4m

# Position and maximum length of espfs in flash memory
ESPFS_POS = 0x20000
ESPFS_SIZE = 0x16000

# -------------- End of esphttpd config options -------------


# Output directors to store intermediate compiled files
# relative to the project directory
BUILD_BASE	= build
FW_BASE		= firmware

# Base directory for the compiler. Needs a / at the end; if not set it'll use the tools that are in
# the PATH.
XTENSA_TOOLS_ROOT ?= /opt/esp-open-sdk/xtensa-lx106-elf/bin/

# base directory of the ESP8266 SDK package, absolute
SDK_BASE	?= /opt/esp-open-sdk/sdk

# Esptool.py path and port
ESPTOOL		?= /opt/esp-open-sdk/esptool/esptool.py
ESPPORT		?= /dev/ttyUSB0
# ESPDELAY indicates seconds to wait between flashing the two binary images
ESPDELAY	?= 3
ESPBAUD		?= 115200

# name for the target project
TARGET		= espweather

# version strings
VERSION 	= $(shell cat VERSION)
USERNAME	= $(shell whoami)
HOSTNAME	= $(shell hostname)

# which modules (subdirectories) of the project to include in compiling
MODULES		= lib/espfs lib/httpd lib/httpclient lib/mqtt user
EXTRA_INCDIR	= include \
		  . \
		  lib/heatshrink/ \
		  lib/httpclient/

# libraries used in this project, mainly provided by the SDK
LIBS		= c gcc hal phy pp net80211 lwip wpa main

# compiler flags using during compilation of source files
CFLAGS		= -Os -std=c99 -Wpointer-arith -Wundef -Werror -Wl,-EL -fno-inline-functions \
		  -nostdlib -mlongcalls -mtext-section-literals -D__ets__ -DICACHE_FLASH \
		  -Wno-address -DESPFS_POS=$(ESPFS_POS) -DESPFS_SIZE=$(ESPFS_SIZE) \
		  -DVERSTRING=\"$(VERSION)\" -DHOSTSTRING=\"$(USERNAME)@$(HOSTNAME)\"

# extra options for ssl support
ifeq ("$(ENABLE_SSL)","yes")
LIBS		+= ssl
CFLAGS		+= -DENABLE_SSL
endif

# linker flags used to generate the main object file
LDFLAGS		= -nostdlib -Wl,--no-check-sections -u call_user_start -Wl,-static

# linker script used for the above linkier step
LD_SCRIPT	= eagle.app.v6.ld

# various paths from the SDK used in this project
SDK_LIBDIR	= lib
SDK_LDDIR	= ld
SDK_INCDIR	= include include/json

# select which tools to use as compiler, librarian and linker
CC		:= $(XTENSA_TOOLS_ROOT)xtensa-lx106-elf-gcc
AR		:= $(XTENSA_TOOLS_ROOT)xtensa-lx106-elf-ar
LD		:= $(XTENSA_TOOLS_ROOT)xtensa-lx106-elf-gcc
SIZE		:= $(XTENSA_TOOLS_ROOT)xtensa-lx106-elf-size


####
#### no user configurable options below here
####
SRC_DIR		:= $(MODULES)
BUILD_DIR	:= $(addprefix $(BUILD_BASE)/,$(MODULES))

SDK_LIBDIR	:= $(addprefix $(SDK_BASE)/,$(SDK_LIBDIR))
SDK_INCDIR	:= $(addprefix -I$(SDK_BASE)/,$(SDK_INCDIR))

SRC		:= $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.c))
OBJ		:= $(patsubst %.c,$(BUILD_BASE)/%.o,$(SRC))
LIBS		:= $(addprefix -l,$(LIBS))
APP_AR		:= $(addprefix $(BUILD_BASE)/,$(TARGET)_app.a)
TARGET_OUT	:= $(addprefix $(BUILD_BASE)/,$(TARGET).out)

LD_SCRIPT_ORIG	:= $(addprefix $(SDK_BASE)/$(SDK_LDDIR)/,$(LD_SCRIPT))
LD_SCRIPT_NEW	:= $(addprefix $(BUILD_BASE)/,$(LD_SCRIPT))
LD_SCRIPT	:= -T$(LD_SCRIPT_NEW)

INCDIR	:= $(addprefix -I,$(SRC_DIR))
EXTRA_INCDIR	:= $(addprefix -I,$(EXTRA_INCDIR))
MODULE_INCDIR	:= $(addsuffix /include,$(INCDIR))

V ?= $(VERBOSE)
ifeq ("$(V)","1")
Q :=
vecho := @true
else
Q := @
vecho := @echo
endif

ifeq ("$(GZIP_COMPRESSION)","yes")
CFLAGS		+= -DGZIP_COMPRESSION
endif

ifeq ("$(USE_HEATSHRINK)","yes")
CFLAGS		+= -DESPFS_HEATSHRINK
endif

vpath %.c $(SRC_DIR)

define compile-objects
$1/%.o: %.c
	$(vecho) "CC $$<"
	$(Q) $(CC) $(INCDIR) $(MODULE_INCDIR) $(EXTRA_INCDIR) $(SDK_INCDIR) $(CFLAGS)  -c $$< -o $$@
endef

.PHONY: all checkdirs clean $(FW_BASE)/webpages.espfs

all: checkdirs $(TARGET_OUT) $(FW_BASE) size

$(LD_SCRIPT_NEW):
	$(vecho) "GEN $@"
	$(Q) cat $(LD_SCRIPT_ORIG) | sed "s/irom0_0_seg : .*/irom0_0_seg :                         org = 0x40236000, len = 0x44000/g" > $(LD_SCRIPT_NEW)

$(TARGET_OUT): $(APP_AR) $(LD_SCRIPT_NEW)
	$(vecho) "LD $@"
	$(Q) $(LD) -L$(SDK_LIBDIR) $(LD_SCRIPT) $(LDFLAGS) -Wl,--start-group $(LIBS) $(APP_AR) -Wl,--end-group -o $@

$(FW_BASE): $(TARGET_OUT)
	$(vecho) "FW $@"
	$(Q) mkdir -p $@
	$(Q) $(ESPTOOL) elf2image $(TARGET_OUT) --output $@/

$(APP_AR): $(OBJ)
	$(vecho) "AR $@"
	$(Q) $(AR) cru $@ $^

checkdirs: $(BUILD_DIR)

$(BUILD_DIR):
	$(Q) mkdir -p $@

size:
	$(Q) $(SIZE) -t $(APP_AR)

flash: checkdirs $(TARGET_OUT) $(FW_BASE)
	$(Q) $(ESPTOOL) --port $(ESPPORT) --baud $(ESPBAUD) write_flash --flash_size $(FLASH_SIZE) 0x00000 $(FW_BASE)/0x00000.bin 0x36000 $(FW_BASE)/0x36000.bin

$(FW_BASE)/webpages.espfs: html/ html/admin/ lib/espfs/mkespfsimage/mkespfsimage
ifeq ("$(COMPRESS_W_YUI)","yes")
	$(Q) rm -rf html_compressed;
	$(Q) cp -r html html_compressed;
	$(Q) echo "Compression assets with yui-compressor. This may take a while..."
	$(Q) for file in `find html_compressed -type f -name "*.js"`; do $(YUI-COMPRESSOR) --type js $$file -o $$file; done
	$(Q) for file in `find html_compressed -type f -name "*.css"`; do $(YUI-COMPRESSOR) --type css $$file -o $$file; done
	$(Q) awk "BEGIN {printf \"YUI compression ratio was: %.2f%%\\n\", (`du -b -s html_compressed/ | sed 's/\([0-9]*\).*/\1/'`/`du -b -s html/ | sed 's/\([0-9]*\).*/\1/'`)*100}"

# mkespfsimage will compress html, css and js files with gzip by default if enabled
# override with -g cmdline parameter
	$(Q) cd html_compressed; find  | ../lib/espfs/mkespfsimage/mkespfsimage > ../$(FW_BASE)/webpages.espfs; cd ..;
else
	$(Q) cd html; find | ../lib/espfs/mkespfsimage/mkespfsimage > ../$(FW_BASE)/webpages.espfs; cd ..
endif

defaultflash:
ifeq ("$(FLASH_SIZE)","4m")
	$(Q) $(ESPTOOL) --port $(ESPPORT) --baud $(ESPBAUD) write_flash --flash_size $(FLASH_SIZE) 0x7B000 $(SDK_BASE)/bin/blank.bin
	$(Q) $(ESPTOOL) --port $(ESPPORT) --baud $(ESPBAUD) write_flash --flash_size $(FLASH_SIZE) 0x7C000 $(SDK_BASE)/bin/esp_init_data_default.bin
	$(Q) $(ESPTOOL) --port $(ESPPORT) --baud $(ESPBAUD) write_flash --flash_size $(FLASH_SIZE) 0x7E000 $(SDK_BASE)/bin/blank.bin
else
	$(Q) $(ESPTOOL) --port $(ESPPORT) --baud $(ESPBAUD) write_flash --flash_size $(FLASH_SIZE) 0xFB000 $(SDK_BASE)/bin/blank.bin
	$(Q) $(ESPTOOL) --port $(ESPPORT) --baud $(ESPBAUD) write_flash --flash_size $(FLASH_SIZE) 0xFC000 $(SDK_BASE)/bin/esp_init_data_default.bin
	$(Q) $(ESPTOOL) --port $(ESPPORT) --baud $(ESPBAUD) write_flash --flash_size $(FLASH_SIZE) 0xFE000 $(SDK_BASE)/bin/blank.bin
endif

lib/espfs/mkespfsimage/mkespfsimage: lib/espfs/mkespfsimage/
	$(Q) $(MAKE) -C lib/espfs/mkespfsimage USE_HEATSHRINK="$(USE_HEATSHRINK)" GZIP_COMPRESSION="$(GZIP_COMPRESSION)"

htmlflash: $(FW_BASE)/webpages.espfs
	$(Q) if [ $$(stat -c '%s' $(FW_BASE)/webpages.espfs) -gt $$(( $(ESPFS_SIZE) )) ]; then echo "webpages.espfs too big!"; false; fi
	$(Q) $(ESPTOOL) --port $(ESPPORT) --baud $(ESPBAUD) write_flash --flash_size $(FLASH_SIZE) $(ESPFS_POS) $(FW_BASE)/webpages.espfs

image: checkdirs $(TARGET_OUT) $(FW_BASE) $(FW_BASE)/webpages.espfs
ifeq ("$(FLASH_SIZE)","4m")
	$(Q) dd if=/dev/zero count=$$((16#80000)) iflag=count_bytes status=none | tr "\000" "\377" > $(FW_BASE)/firmware.bin
else
	$(Q) dd if=/dev/zero count=$$((16#100000)) iflag=count_bytes status=none | tr "\000" "\377" > $(FW_BASE)/firmware.bin
endif
	$(Q) dd if=$(FW_BASE)/0x00000.bin of=$(FW_BASE)/firmware.bin seek=$$((16#00000)) oflag=seek_bytes conv=notrunc status=none
	$(Q) dd if=$(FW_BASE)/webpages.espfs of=$(FW_BASE)/firmware.bin seek=$$((16#`echo $(ESPFS_POS) | cut -c 3-`)) oflag=seek_bytes conv=notrunc status=none
	$(Q) dd if=$(FW_BASE)/0x36000.bin of=$(FW_BASE)/firmware.bin seek=$$((16#36000)) oflag=seek_bytes conv=notrunc status=none
ifeq ("$(FLASH_SIZE)","4m")
	$(Q) dd if=$(SDK_BASE)/bin/esp_init_data_default.bin of=$(FW_BASE)/firmware.bin seek=$$((16#7C000)) oflag=seek_bytes conv=notrunc status=none
else
	$(Q) dd if=$(SDK_BASE)/bin/esp_init_data_default.bin of=$(FW_BASE)/firmware.bin seek=$$((16#FC000)) oflag=seek_bytes conv=notrunc status=none
endif

clean:
	$(Q) rm -f $(APP_AR)
	$(Q) rm -f $(TARGET_OUT)
	$(Q) find $(BUILD_BASE) -type f | xargs rm -f
	$(Q) find $(BUILD_BASE) -maxdepth 1 -type d ! -name "$(BUILD_BASE)" | xargs rm -rf
	$(Q) make -C lib/espfs/mkespfsimage/ clean
	$(Q) rm -rf $(FW_BASE)
ifeq ("$(COMPRESS_W_YUI)","yes")
	$(Q) rm -rf html_compressed
endif

$(foreach bdir,$(BUILD_DIR),$(eval $(call compile-objects,$(bdir))))
