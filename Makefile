#/*****************************************************************************
#* Copyright © 2019-2020 Mike Sharkey <mike@8bitgeek.net>                     *
#*                                                                            *
#* Permission is hereby granted, free of charge, to any person obtaining a    *
#* copy of this software and associated documentation files (the "Software"), *
#* to deal in the Software without restriction, including without limitation  *
#* the rights to use, copy, modify, merge, publish, distribute, sublicense,   *
#* and/or sell copies of the Software, and to permit persons to whom the      *
#* Software is furnished to do so, subject to the following conditions:       *
#*                                                                            *
#* The above copyright notice and this permission notice shall be included in *
#* all copies or substantial portions of the Software.                        *
#*                                                                            *
#* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR *
#* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   *
#* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    *
#* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
#* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    *
#* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        *
#* DEALINGS IN THE SOFTWARE.                                                  *
#*****************************************************************************/
TARGET=libezbus.a

LIBEZBUS_UDP_SRC=../libezbus_udp/src

PREFIX=/usr/bin/

CC=$(PREFIX)gcc
LD=$(PREFIX)gcc
AR=$(PREFIX)ar
AS=$(PREFIX)as
CP=$(PREFIX)objcopy
OD=$(PREFIX)objdump
SE=$(PREFIX)size
RL=$(PREFIX)ranlib

ARFLAGS = rcs
CFLAGS += -c
CFLAGS += -std=gnu99 -ggdb -O0 -Wall -Wno-unused-function
LFLAGS = -Wl,-Map=$(TARGET).map

INCLUDE =  -I ./
INCLUDE += -I ./src/platform/linux -I $(LIBEZBUS_UDP_SRC)
INCLUDE += -I ./src -I ./src/mac -I ./src/common -I ./src/socket -I ./src/platform

C_SRC  += src/ezbus.c

C_SRC  += src/mac/ezbus_mac_arbiter.c
C_SRC  += src/mac/ezbus_mac_arbiter_pause.c
C_SRC  += src/mac/ezbus_mac_arbiter_receive.c
C_SRC  += src/mac/ezbus_mac_arbiter_transmit.c
C_SRC  += src/mac/ezbus_mac_boot0.c
C_SRC  += src/mac/ezbus_mac_boot1.c
C_SRC  += src/mac/ezbus_mac_boot2.c
C_SRC  += src/mac/ezbus_mac.c
C_SRC  += src/mac/ezbus_mac_pause.c
C_SRC  += src/mac/ezbus_mac_peers.c
C_SRC  += src/mac/ezbus_mac_receiver.c
C_SRC  += src/mac/ezbus_mac_timer.c
C_SRC  += src/mac/ezbus_mac_token.c
C_SRC  += src/mac/ezbus_mac_transmitter.c

C_SRC  += src/common/ezbus_address.c
C_SRC  += src/common/ezbus_crc32.c
C_SRC  += src/common/ezbus_crc.c
C_SRC  += src/common/ezbus_fault.c
C_SRC  += src/common/ezbus_flip.c
C_SRC  += src/common/ezbus_hex.c
C_SRC  += src/common/ezbus_log.c
C_SRC  += src/common/ezbus_packet.c
C_SRC  += src/common/ezbus_parcel.c
C_SRC  += src/common/ezbus_pause.c
C_SRC  += src/common/ezbus_peer.c
C_SRC  += src/common/ezbus_port.c
#C_SRC  += src/common/ezbus_string.c

C_SRC  += src/socket/ezbus_socket.c
C_SRC  += src/socket/ezbus_socket_callback.c
C_SRC  += src/socket/ezbus_socket_common.c

C_SRC  += src/platform/ezbus_platform.c

# Object files to build.
OBJS  = $(AS_SRC:.S=.o)
OBJS += $(C_SRC:.c=.o)

# Default rule to build the whole project.
.PHONY: all
all: $(TARGET)

# Rule to build assembly files.
%.o: %.S
	$(CC) -x assembler-with-cpp $(ASFLAGS) $(INCLUDE) $< -o $@

# Rule to compile C files.
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) $< -o $@

# Rule to create an ELF file from the compiled object files.
$(TARGET): $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)
	$(RL) $@

clean:
		rm -f $(OBJS) $(TARGET)

