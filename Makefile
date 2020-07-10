######################################
# target
######################################
TARGET_F0 = EO94_IC_ABB_ACS

######################################
# building variables
######################################
# debug build?
DEBUG = 1
# optimization
OPT = -O0
CPPSTD = -std=c++17

# Build path
BUILD_DIR = build

######################################
# source
######################################
# LIBRARY_PATH = /net/factory/share/projects/code/mculib2
# LIBRARY_PATH = /net/factory/users/dvk/code/mculib2
LIBRARY_PATH = mculib2
MCULIB_VERSION = d14308ac97e448953aaaccac59ea6b82a5919139

CPP_SOURCES_F0 = src/main.cpp

ASM_SOURCES_F0 = $(LIBRARY_PATH)/STM32F0_files/startup_stm32f030x6.s

# C includes
C_INCLUDES =  
C_INCLUDES += -Isrc
# C_INCLUDES += -I$(BOOST_ROOT)
# C_INCLUDES += -I$(BOOST_ROOT)\stage\lib64
C_INCLUDES += -I$(LIBRARY_PATH)
C_INCLUDES += -I$(LIBRARY_PATH)/hal
C_INCLUDES += -I$(LIBRARY_PATH)/hal/ral

C_INCLUDES_F0 =
C_INCLUDES_F0 += -I$(LIBRARY_PATH)/STM32F0_files 
C_INCLUDES_F0 += -I$(LIBRARY_PATH)/STM32F0_files/CMSIS 



#######################################
# binaries
#######################################
PREFIX = arm-none-eabi-

CPP = $(PREFIX)g++
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
AR = $(PREFIX)ar
SZ = $(PREFIX)size
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S
 
#######################################
# CFLAGS
#######################################
CPU_F0 = -mcpu=cortex-m0

# NONE for Cortex-M0/M0+/M3
FPU_F0 =

FLOAT-ABI_F0 =

# mcu
MCU_F0 = $(CPU_F0) -mthumb $(FPU_F0) $(FLOAT-ABI_F0)

# compile gcc flags
ASFLAGS_F0 = $(MCU_F0) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS_F0  = $(MCU_F0) $(C_DEFS_F0) $(C_INCLUDES) $(C_INCLUDES_F0) $(OPT)
CFLAGS_F0 += -Wall -Wno-register -fdata-sections -ffunction-sections -fno-exceptions -fno-strict-volatile-bitfields

CFLAGS_F0 += -g -gdwarf-2 


# Generate dependency information
CFLAGS_F0 += -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)"

#######################################
# LDFLAGS
#######################################
LDSCRIPT_F0 = $(LIBRARY_PATH)/STM32F0_files/STM32F030K6Tx_FLASH.ld

# libraries
LIBS = -lc -lm -lnosys 

LDFLAGS_F0  = $(MCU_F0) -specs=nano.specs -specs=nosys.specs
LDFLAGS_F0 += -T$(LDSCRIPT_F0) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET_F0).map,--cref -Wl,--gc-sections

# default action: build all
all: clean  \
$(BUILD_DIR)/$(TARGET_F0).elf $(BUILD_DIR)/$(TARGET_F0).hex $(BUILD_DIR)/$(TARGET_F0).bin
	

#######################################
# build the application
#######################################
# list of objects
# OBJECTS_F0 = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES_F0:.c=.o)))
# vpath %.c $(sort $(dir $(C_SOURCES_F0)))
# OBJECTS_F0 += $(addprefix $(BUILD_DIR)/,$(notdir $(CPP_SOURCES_F0:.cpp=.o)))
# vpath %.cpp $(sort $(dir $(CPP_SOURCES_F0)))
OBJECTS_F0 += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES_F0:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES_F0)))


$(BUILD_DIR)/main.o: $(CPP_SOURCES_F0) Makefile | $(BUILD_DIR) 
	$(CPP) -c $(CFLAGS_F0) $(CPPSTD) -fno-rtti -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.cpp=.lst)) $< -o $@

$(BUILD_DIR)/startup_stm32f030x6.o: $(ASM_SOURCES_F0) Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS_F0) $< -o $@

$(BUILD_DIR)/$(TARGET_F0).elf: $(OBJECTS_F0) build/main.o Makefile
	$(CPP) $(OBJECTS_F0) build/main.o $(LDFLAGS_F0) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@
	
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@	
	
$(BUILD_DIR):
	mkdir $@		

#######################################
# clean up
#######################################
clean:
	-rm -fR .dep $(BUILD_DIR)


flash:
	st-flash write $(BUILD_DIR)/$(TARGET_F0).bin 0x8000000

eo85_flash:
	st-flash write $(BUILD_DIR)/EO85_zevs.bin 0x8000000

util:
	#/home/dvk/code/stlink/build/Release/src/gdbserver/st-util
	#/home/slonegd/Code/stlink/build/Release/src/gdbserver/st-util
	#/home/peltikhin/code/EmbeddedArm/stlink/build/Release/src/gdbserver/st-util
	st-util

submodule:

	git submodule update --init

	cd mculib2/ && git fetch

	cd mculib2/ && git checkout $(MCULIB_VERSION)

	cd mculib2/ && git submodule update --init

 
test_:
	$(MAKE) -I $(LIBRARY_PATH) -C ./test/

#######################################
# dependencies
#######################################
-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)

# *** EOF ***
