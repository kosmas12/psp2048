TARGET = 2048-PSP
OBJS = src/psp2048.o src/callbacks.o src/graphics.o src/framebuffer.o src/input.o


INCDIR =
CFLAGS = -O2 -G0 -Wall -g -I include
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
CXXFLAGS += -Wall -Wuninitialized -O -DLINUX
ASFLAGS = $(CFLAGS)

LIBDIR =
LDFLAGS = 
LIBS = -lintraFont -lpspgum -lpspgu -lpng -lz -lm -lpsprtc

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = 2048-PSP
PSP_EBOOT_ICON = ICON0.PNG
PSP_EBOOT_PIC1 = PIC1.PNG

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

