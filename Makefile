TARGET   ?= project_1
AUTHOR   ?= Elementro
MCU      ?= STM32H723xx

#=============== ФЛАГИ отладки ===========================
C_FLAGS     = -O2 -g3
ASM_FLAGS   = -g3
LD_FLAGS    =
#=========================================================

#=============== arm-none-eabi ============================
PREFIX      = arm-none-eabi
CC          = $(PREFIX)-gcc
OBJCOPY     = $(PREFIX)-objcopy
SIZE        = $(PREFIX)-size
#=========================================================

#=============== MCU ФЛАГИ ===============================
MCU_FLAGS   = -mthumb \
              -mcpu=cortex-m7 \
              -mfloat-abi=hard \
              -mfpu=fpv5-d16

C_FLAGS    += -mthumb -Wall -Wextra -std=gnu11 $(MCU_FLAGS) -D$(MCU)
LD_FLAGS   += -mthumb $(MCU_FLAGS) -Tlinker/linker.ld -Wl,--gc-sections -nostartfiles
ASM_FLAGS  += -mthumb $(MCU_FLAGS)
#=========================================================

#=============== ФАЙЛЫ ===================================
BUILD_DIR = build

C_SRC   = src/main.c \
          src/syscalls.c   # Добавьте syscalls.c

ASM_SRC = startup/startup_stm32h723vgtx.s

ASM_OBJ = $(ASM_SRC:%.s=$(BUILD_DIR)/%.o)
C_OBJ   = $(C_SRC:%.c=$(BUILD_DIR)/%.o)
OBJS    = $(ASM_OBJ) $(C_OBJ)
#=========================================================

#=============== ИНКЛЮДЫ =================================
INCLUDES = -Iinc \
           -ICMSIS/Include \
           -ICMSIS/Device/ST/STM32H7xx/Include
#=========================================================

#=============== ЦЕЛИ ====================================
all: $(BUILD_DIR)/$(TARGET).elf
	@echo "Project name: $(TARGET)"
	@echo "Creator:      $(AUTHOR)"

$(BUILD_DIR)/%.o : %.s
	@mkdir -p $(dir $@)
	$(CC) $(ASM_FLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/%.o : %.c
	@mkdir -p $(dir $@)
	$(CC) $(C_FLAGS) $(INCLUDES) -c $< -o $@

$(BUILD_DIR)/$(TARGET).elf : $(OBJS)
	$(CC) $(LD_FLAGS) -o $@ $(OBJS)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean