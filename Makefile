PATH := $(DEVKITARM)/bin:$(PATH)

PROJ    := main
TARGET  := $(PROJ)

OBJS    := $(PROJ).o

PREFIX  := arm-none-eabi-
CC      := $(PREFIX)gcc
LD      := $(PREFIX)gcc
OBJCOPY := $(PREFIX)objcopy

ARCH    := -mthumb-interwork -mthumb
SPECS   := -specs=gba.specs

CFLAGS  := $(ARCH) -O2 -Wall -fno-strict-aliasing
LDFLAGS := $(ARCH) $(SPECS)

EXECUTABLE := "C:/Program Files (x86)/mGBA/mGBA.exe"

build: $(TARGET).gba run

$(TARGET).gba : $(TARGET).elf
	$(OBJCOPY) -v -O binary $< $@
	-@gbafix $@

$(TARGET).elf : $(OBJS)
	$(LD) $^ $(LDFLAGS) -o $@

$(OBJS) : %.o : %.c
	$(CC) -c $< $(CFLAGS) -o $@

.PHONY : clean
clean :
	@rm -fv *.gba
	@rm -fv *.elf
	@rm -fv *.o

.PHONY : run
run:
	$(EXECUTABLE) C:/Users/Jasper/CLionProjects/gba/main.elf