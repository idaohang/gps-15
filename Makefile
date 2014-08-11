
BINS = gps

#router
CURRENTDIR = ${shell pwd}
DESTBIN ?=  bin
DESTLIB ?= lib
BASICLIBDIR := $(CURRENTDIR)/../libhd
PYTHONLIBDIR:=$(CURRENTDIR)/../../../update_files/usr
SSLLIBPATH:=$(CURRENTDIR)/../../../firmware/rootfs/usr/lib/arm-linux-gnueabi

#编译环境相关，为了单目录编译
CROSS_COMPILE ?= armv7l-timesys-linux-gnueabi-
CC := $(CROSS_COMPILE)gcc
AR := $(CROSS_COMPILE)ar
AS := $(CROSS_COMPILE)as
LD := $(CROSS_COMPILE)ld
NM := $(CROSS_COMPILE)nm
RANLIB := $(CROSS_COMPILE)ranlib
STRIP := $(CROSS_COMPILE)strip
SIZE := $(CROSS_COMPILE)size
MAKE = make

CFLAGS = -g -O2 -Wall -Dlinux -D__linux__ -Dunix -DEMBED  -I$(BASICLIBDIR)# -I$(PYTHONLIBDIR)/include/python2.7
LDFLAGS	+=  -lhdg3 -L$(BASICLIBDIR)   -D REENTRANT -lcrypt -ldl 

#补丁版本号相关
COMPILE_PATCH ?= yes
PATCH_DATE ?= `date +%y%m%d`
ifeq ($(COMPILE_PATCH), yes)
COMPILE_PATCH_VERSION=$(COMPILE_VERSION).1_${PATCH_DATE}
endif

CFLAGS += -DBIN_VERSION=\"$(COMPILE_PATCH_VERSION)\"

SRCS := $(wildcard ./*.c)
OBJS := $(subst .c,.o,$(SRCS))
all: ${BINS}
${BINS}: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)
$(OBJS):%.o:%.c
	$(CC) -c $(CFLAGS) $< -o $@
clean:
	rm -f *.[oa] *.elf *~ core ${BINS} *.gdb *.orig
install: all
	install -d ${DESTBIN}
	install -m 755 ${BINS} ${DESTBIN} 
	$(STRIP) ${DESTBIN}/${BINS}

.PHONY:install all clean 
