TARGET	?= project_1
AUTHOR 	?= Elementro
MCU	?=STM32H723xx

#=============== ФАЛГИ отладки ===============================
C_FLAGS	 = -O2 -g3
ASM_FLAGS= -g3
LD_FLAGS =
#=============================================================

#=============== arm-none-eabi ===============================
PREFIX	= arm-none-eabi
CC	= $(PREFIX)-gcc
OBJCOPY	= $(PREFIX)-objcopy
SIZE 	= $(PREFIX)-size
#==============================================================


#============================= ФЛАГ ===========================
MCU_FLAGS= \
	   -mthumb \
	   -mcpu=cortex-m7 \
	   -mfloat-abi=hard \
	   -mfpu=fpv5-d16

C_FLAGS	  += -mthumb -Wall -Wextra -std=gnu11 $(MCU_FLAGS) -D$(MCU)
LD_FLAGS  += -mthumb $(MCU_FLAGS)
ASM_FLAGS += -mthumb $(MCU_FLAGS) 

#==============================================================


#============================= ФАЙЛЫ ==========================
#Сюда вписывать сурсы
BUILD_DIR= build
C_SRC	= \
	  src/main.c 
#ТУТА СТАРТАП
ASM_SRC	= \
	  startup/startup_stm32h723vgtx.s

ASM_OBJ	= $(ASM_SRC:%.s=$(BUILD_DIR)/%.o)
C_OBJ	= $(C_SRC:%.c=$(BUILD_DIR)/%.o)

OBJS 	= $(ASM_OBJ) $(C_OBJ)


#================================================================

#============================== ИНКЛЮДЫ =========================
#Сюда вписывать инклюды
INCLUDES= \
	  -Iinc\
	  -ICMSIS/Include \
	  -ICMSIS/Device/ST/STM32H7xx/Include
#================================================================
all: 	$(OBJS)
	@echo Project name: 	$(TARGET)
	@echo Creator: 		$(AUTHOR)

$(BUILD_DIR)/%.o : %.s
	@mkdir -p $(dir $@)
	$(CC) $(ASM_FLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/%.o : %.c
	@mkdir -p $(dir $@)
	$(CC) $(C_FLAGS) $(INCLUDES) -c $< -o $@
	
clean:
	rm -rf	$(BUILD_DIR)
