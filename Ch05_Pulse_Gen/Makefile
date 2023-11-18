PROJ=Ch05_Pulse_Gen
DBG = TRUE

# Compiler variables
CC = arm-none-eabi-gcc
CFLAGS = -mthumb -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -std=gnu11 -DUSE_HAL_DRIVER -DSTM32G431xx
SFLAGS = $(CFLAGS) -x assembler-with-cpp
OBJ_FLAGS = -c -O2 -ffunction-sections
EXE_FLAGS = -Tlinker_script.ld -Wl,--gc-sections -static
LIBS = -lc -lm -lrdimon

# Source directories
SRC_DIR = Core
HAL_SRC_DIR = Drivers/STM32G4xx_HAL_Driver/Src
OBJ_DIR = Objects

# Include directories
INC_DIRS = $(SRC_DIR)
INC_DIRS += Drivers/STM32G4xx_HAL_Driver/Inc
INC_DIRS += Drivers/STM32G4xx_HAL_Driver/Inc/Legacy
INC_DIRS += Drivers/CMSIS/Include
INC_DIRS += Drivers/CMSIS/Device/ST/STM32G4xx/Include

# File variables
SRC_FILES = $(wildcard $(SRC_DIR)/*.c $(HAL_SRC_DIR)/*.c)
S_FILES = $(wildcard $(SRC_DIR)/*.s)
OBJ_FILES = $(patsubst %.c, $(OBJ_DIR)/%.o, $(notdir $(SRC_FILES)))
OBJ_FILES += $(patsubst %.s, $(OBJ_DIR)/%.o, $(notdir $(S_FILES)))

# Debug setting - set to TRUE to enable debugging
ifeq ($(DBG), TRUE)
	CFLAGS += -g3 -DDBG
	SRC_FILES += Debug/print_debug.c
	DEBUG_DIR = Debug
	INC_DIRS += Debug
else
	CFLAGS += -O2
endif

# Primary target
all: $(PROJ).elf

# Build executable
$(PROJ).elf: $(OBJ_FILES)
	$(CC) -o $@ $(CFLAGS) $(EXE_FLAGS) $^  $(LIBS)

# Compile source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -o $@ $(CFLAGS) $(OBJ_FLAGS) $(INC_DIRS:%=-I%) $<

# Compile HAL files
$(OBJ_DIR)/%.o: $(HAL_SRC_DIR)/%.c
	$(CC) -o $@ $(CFLAGS) $(OBJ_FLAGS) $(INC_DIRS:%=-I%) $<

# Compile Debug files
$(OBJ_DIR)/%.o: $(DEBUG_DIR)/%.c
	$(CC) -o $@ $(CFLAGS) $(OBJ_FLAGS) $(INC_DIRS:%=-I%) $<

# Compile assembly files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.s
	$(CC) -o $@ $(CFLAGS) $(INC_DIRS:%=-I%) -c $<

# Other Targets
clean:
	rm -rf $(PROJ).elf $(OBJ_DIR)/*.o

.PHONY: all clean