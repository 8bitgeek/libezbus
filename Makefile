#******************************************************************************
#* Copyright © 2018 by @author Mike Sharkey <mike@8bitgeek.net>
#* All Rights Reserved
#*****************************************************************************/
TARGET=ezapp

PREFIX=/usr/bin/

CC=$(PREFIX)gcc
LD=$(PREFIX)gcc
AR=$(PREFIX)ar
AS=$(PREFIX)as
CP=$(PREFIX)objcopy
OD=$(PREFIX)objdump
SE=$(PREFIX)size

CFLAGS  = -std=gnu99 -ggdb -O0 -Wall
LFLAGS = -Wl,-Map=$(TARGET).map

INCLUDE =  -I ./
INCLUDE += -I ./ezbus/src -I ./ezbus/src/platform/linux
INCLUDE += -I ./ezbus_tl/src -I ./ezbus_tl/src/platform

SRCS += ezbus/src/platform/linux/ezbus_thread.c
SRCS += ezbus/src/platform/linux/ezbus_signal.c
SRCS += ezbus/src/platform/linux/fault.c
SRCS += ezbus/src/platform/linux/main.c

SRCS += ezbus_tl/src/ezbus_address.c
SRCS += ezbus_tl/src/ezbus_crc.c
SRCS += ezbus_tl/src/ezbus_instance.c
SRCS += ezbus_tl/src/ezbus_packet.c
SRCS += ezbus_tl/src/ezbus_packet_queue.c
SRCS += ezbus_tl/src/ezbus_platform.c
SRCS += ezbus_tl/src/ezbus_port.c

$(TARGET): $(SRCS) 
		$(CC) $(LFLAGS) $(INCLUDE) $(CFLAGS) $^ -o $@

clean:
		rm -f *.o $(TARGET) $(TARGET).map

