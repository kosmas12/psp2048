TARGET = 2048-PSP
OBJS = src/psp2048.o src/callbacks.o src/graphics.o src/framebuffer.o src/input.o

PSP_LARGE_MEMORY = 1

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = 2048-PSP
PSP_EBOOT_ICON = ICON0.PNG
PSP_EBOOT_PIC1 = PIC1.PNG

INCDIR += include
CFLAGS = -Og -Wall -Wextra -g
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
CXXFLAGS += $(CFLAGS)
ASFLAGS = $(CFLAGS)

LIBS += -lpspgum -lpspgu -lpng -lz -lm -lpsprtc
PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

