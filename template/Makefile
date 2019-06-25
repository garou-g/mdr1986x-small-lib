# Makefile for MDR1986VE1T bootloader by MIL-STD1553
# Have two parts: bootloader and bootloader_ram

# Compiler prefix --------------------------------------------------------------

PREFIX = arm-none-eabi-
ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
else
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
endif

# Hex and bin convertors commands
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S

# Target -----------------------------------------------------------------------

TARGET = bootloader
TARGET_RAM = bootloader_ram

# Debug build?
DEBUG = 1

# Optimization
OPT = -O0

BUILD_DIR = build

# Sources, includes and defines ------------------------------------------------

C_SOURCES = \
source/main.c \
source/boot.c \
source/system.c \

ASM_SOURCES =  \
libs/startup_gcc_MDR1986VE1T.s

C_SOURCES_RAM = \
source/boot.c \
source/MDR32F9Qx_eeprom.c

ASM_SOURCES_RAM =  \
libs/startup_gcc_MDR1986VE1T_RAM.s

AS_INCLUDES =

C_INCLUDES = \
-Iinclude \
-Ilibs

AS_DEFS =

C_DEFS = -DUSE_MDR1986VE1T -D__NO_SYSTEM_INIT

C_DEFS_RAM = $(C_DEFS) -DRAM_BOOT

# CPU --------------------------------------------------------------------------

CPU = -mcpu=cortex-m1
FPU = #-mfpu=fpv4-sp-d16
FLOAT-ABI = #-mfloat-abi=hard

MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# Compiler flags ---------------------------------------------------------------

ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) \
-Wall -fdata-sections -ffunction-sections

CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) \
-Wall -fdata-sections -ffunction-sections

CFLAGS_RAM = $(MCU) $(C_DEFS_RAM) $(C_INCLUDES) $(OPT) \
-Wall -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-3
endif

# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"
CFLAGS_RAM += -MMD -MP -MF"$(@:%.o=%.d)"

# Linker flags -----------------------------------------------------------------

# Linker script
LDSCRIPT = libs/MDR1986VE1T.ld
LDSCRIPT_RAM = libs/MDR1986VE1T_RAM.ld

# Libraries
LIBS = -lc -lm -lnosys
LIBDIR =

LDFLAGS = $(MCU) -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -specs=nano.specs \
-Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

LDFLAGS_RAM = $(MCU) -T$(LDSCRIPT_RAM) $(LIBDIR) $(LIBS) -specs=nano.specs \
-Wl,-Map=$(BUILD_DIR)/$(TARGET_RAM).map,--cref -Wl,--gc-sections

# Actions -----------------------------------------------------------------

# List of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))

OBJECTS_RAM = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES_RAM:.c=_ram.o)))
vpath %.c $(sort $(dir $(C_SOURCES_RAM)))

# List of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

OBJECTS_RAM += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES_RAM:.s=_ram.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES_RAM)))


# Actions for specific types of objects
$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%_ram.o: %.c Makefile | $(BUILD_DIR)
	$(CC) -c $(CFLAGS_RAM) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/%_ram.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS_RAM) $< -o $@


# Actions for specific file types
$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/$(TARGET_RAM).elf: $(OBJECTS_RAM) Makefile
	$(CC) $(OBJECTS_RAM) $(LDFLAGS_RAM) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@

$(BUILD_DIR):
	mkdir $@


# Default action: build all
all: all-flash all-ram

all-flash: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin
all-ram: $(BUILD_DIR)/$(TARGET_RAM).elf

# Clean up
clean:
	-rm -fR $(BUILD_DIR)
