################ config ################

CC = arm-none-eabi-gcc -mthumb -mcpu=cortex-m3 -g -Wall -Wextra -O2

SRC = *.c lib/*.c app/**/*.c
HEAD = *.h lib/*.h app/**/*.h
APPINC = $(patsubst %,-I %,$(wildcard app/*))
INC = -I . -I lib
INC += $(APPINC)

# TARGET = STM32F103C6
TARGET = STM32F103C8

# USE_ASSERT = true

############## config end ##############

ifeq ($(TARGET),STM32F103C6)
	LD = -T lib/stm32_flash_ld.ld
	MACRO = -D USE_STDPERIPH_DRIVER -D STM32F10X_LD
	DEVICE = target/stm32f1x.cfg
	STARTUP = lib/startup_stm32f10x_ld.s
endif

ifeq ($(TARGET),STM32F103C8)
	LD = -T lib/stm32_flash_md.ld
	MACRO = -D USE_STDPERIPH_DRIVER -D STM32F10X_MD
	DEVICE = target/stm32f1x.cfg
	STARTUP = lib/startup_stm32f10x_md.s
endif

ifeq ($(USE_ASSERT),true)
	MACRO += -D USE_FULL_ASSERT
endif

LINK = -specs=nano.specs -specs=nosys.specs -static -Wl,-cref,-u,Reset_Handler -Wl,-Map=build/target.map -Wl,--gc-sections -Wl,--defsym=malloc_getpagesize_P=0x80 -Wl,--start-group -lc -lm -Wl,--end-group
PWD = $(shell pwd)

.PHONY: build clean download flash debug size

build : build/target.hex build/target.bin size

build/target.bin : build/target.elf
	arm-none-eabi-objcopy $< $@

build/target.hex : build/target.elf
	arm-none-eabi-objcopy $< -Oihex $@

build/target.elf : $(SRC) $(STARTUP) $(HEAD) makefile
	$(CC) -o $@ $(SRC) $(STARTUP) $(INC) $(LD) $(MACRO) $(LINK)

size :
	arm-none-eabi-size build/target.elf -G

clean : 
	rm build/*

# download via STLINK
download :
	openocd -f interface/stlink-v2.cfg -f $(DEVICE) -c init -c halt -c "flash write_image erase $(PWD)/build/target.bin" -c reset -c shutdown

# download via UART
flash :
	stm32flash -w build/target.hex -v -g 0x0 /dev/ttyUSB0

debug :
	openocd -f interface/stlink-v2.cfg -f $(DEVICE)
