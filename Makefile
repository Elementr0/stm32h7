TARGET	?= project_1



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


DEFS	=\
	 -DSTM32H723xx \
	 -DUSE_FULL_LL_DRIVER


all : 
	@echo	$(TARGET)
