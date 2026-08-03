TARGET	?= project_1
AUTHOR 	?= Elementro


#Сюда вписывать сурсы
SRC	= \	




#Сюда вписывать инклюды
INCLUDES= \
	  -Iinc\
	  -ICMSIS/Include \
	  -ICMSIS/Divice/ST/STM32H7xx/Include

BUILD_DIR	= build

PREFIX 	= arm-none-eabi

CC 	= $(PREFIX)-gcc
OBJCOPY	= $(PREFIX)-objcopy
SIZE	= $(PREFIX)-size


ASM = startup/startup_stm32h723vgtx.s


DEFS	=\
	 -DSTM32H723xx \
	 -DUSE_FULL_LL_DRIVER




all :
	@echo Project name: 	$(TARGET)
	@echo Creator: 		$(AUTHOR)

	@mkdir -p $(BUILD_DIR)

clean:
	rm -rf	$(BUILD_DIR)
