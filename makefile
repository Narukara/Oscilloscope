CC = arm-none-eabi-gcc -mthumb -mcpu=cortex-m3 -g -Wall -Wextra -O1
LINK = -specs=nano.specs -specs=nosys.specs -static -Wl,-cref,-u,Reset_Handler -Wl,-Map=build/target.map -Wl,--gc-sections -Wl,--defsym=malloc_getpagesize_P=0x80 -Wl,--start-group -lc -lm -Wl,--end-group
SRC = *.c lib/*.c app/*.c
HEAD = *.h lib/*.h app/*.h
INC = -I . -I lib -I app
PWD = $(shell pwd)
# 使用不同芯片：更改LD, MACRO, DEVICE, STARTUP
LD = -T lib/stm32_flash_md.ld
MACRO = -D USE_STDPERIPH_DRIVER -D STM32F10X_MD -D USE_FULL_ASSERT
DEVICE = target/stm32f1x.cfg
STARTUP = lib/startup_stm32f10x_md.s

# flash: 串口下载, download: stlink下载
.PHONY: build clean download flash debug size

build : build/target.hex build/target.bin size

build/target.bin : build/target.elf
	arm-none-eabi-objcopy $< $@

build/target.hex : build/target.elf
	arm-none-eabi-objcopy $< -Oihex $@

build/target.elf : $(SRC) $(STARTUP) $(HEAD)
	$(CC) -o $@ $(SRC) $(STARTUP) $(INC) $(LD) $(MACRO) $(LINK)

size :
	arm-none-eabi-size build/target.elf -G

clean : 
	rm build/*

download :
	openocd -f interface/stlink-v2.cfg -f $(DEVICE) -c init -c halt -c "flash write_image erase $(PWD)/build/target.bin" -c reset -c shutdown

flash :
	stm32flash -w build/target.hex -v -g 0x0 /dev/ttyUSB0

debug :
	openocd -f interface/stlink-v2.cfg -f $(DEVICE)