TARGET	?= project_1
AUTHOR 	?= Elementro

MCU	?=STM32H723xx


#Сюда вписывать сурсы
SRC	= \	




#Сюда вписывать инклюды
INCLUDES= \
	  -Iinc\
	  -ICMSIS/Include \
	  

BUILD_DIR	= build

PREFIX 	= arm-none-eabi

CC 	= $(PREFIX)-gcc
OBJCOPY	= $(PREFIX)-objcopy
SIZE	= $(PREFIX)-size

MCU_FLAGS = \
	    -mthumb

ifeq ($(MCU),STM32H723xx)
	ASM_DIR	 = startup/startup_stm32h723vgtx.s
	
	INCLUDES +=-ICMSIS/Device/ST/STM32H7xx/Include\
 
	DEFS	=\
		 -DSTM32H723xx \
		 -DUSE_FULL_LL_DRIVER
	MCU_FLAGS += \
		    -mcpu=cortex-m7 \
		    -mfpu=fpv5-d16 \
		    -mfloat-abi=hard
else
$(error Неизвестный MCU: $(MCU))
endif






all :
	@echo Project name: 	$(TARGET)
	@echo Creator: 		$(AUTHOR)
	@echo $(MCU_FLAGS)	


$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIRi)/%.o : startup/%.s | $(BUILD_DIR)
	$(CC) $

clean:
	rm -rf	$(BUILD_DIR)
