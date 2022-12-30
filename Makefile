# Include the core environment definitions; this will set $(TOP).
# Determine underlying Operating System
ifeq ($(OS),Windows_NT)
	HOST_OS = WINDOWS
else
	WHICH_OS := $(shell uname -s)
	ifeq ($(WHICH_OS),Darwin)
		HOST_OS		= MAC OS
	endif
	ifeq ($(WHICH_OS),CYGWIN_NT-10.0)
		HOST_OS = CYGWIN OS
	endif
	ifeq ($(WHICH_OS),Linux)
		HOST_OS  = LINUX_OS
	endif
endif

include ../../py/mkenv.mk

# Include py core make definitions.
include $(TOP)/py/py.mk
DEVICE_SRC_DIR    	  = ./RTE/Device/DEVICE_M55_HE

# Set makefile-level MicroPython feature configurations.
MICROPY_ROM_TEXT_COMPRESSION ?= 0

# Define toolchain and other tools.
CROSS_COMPILE ?= arm-none-eabi-
DFU ?= $(TOP)/tools/dfu.py
PYDFU ?= $(TOP)/tools/pydfu.py

# Point at required Includes and your base ARM-DS installation
ifneq ($(HOST_OS), LINUX_OS)
USER_BASE    := $(subst \,/,$(USERPROFILE))
INCLUDE_BASE := $(USER_BASE)/AppData/Local/Arm/Packs/ARM/CMSIS/5.9.0
else
USER_BASE    := /home/$(shell whoami)
INCLUDE_BASE := $(USER_BASE)/.cache/arm/packs/ARM/CMSIS/5.9.0
endif

INCLUDES     =																		\
		-I $(INCLUDE_BASE)/CMSIS/Core/Include/CMSIS/Core/Include					\
		-I $(INCLUDE_BASE)/CMSIS/Core/Include										\
		-I $(INCLUDE_BASE)/Device/ARM/ARMCM55/Include								\
		-I $(INCLUDE_BASE)/AlifSemiconductor/Ensemble/0.4.1/Device/M55_HE			\
		-I $(INCLUDE_BASE)/AlifSemiconductor/Ensemble/0.4.1/Device/M55_HE/Config  	\
		-I $(INCLUDE_BASE)/AlifSemiconductor/Ensemble/0.4.1/Device/M55_HE/Include

# Set CFLAGS.
CFLAGS += -I. -I$(TOP) -I$(BUILD) $(INCLUDES)
#CFLAGS += -Wall -Werror -nostdlib
#	-target=arm-arm-none-eabi 	\

CFLAGS += 						\
	-march=armv8.1-m.main		\
	-Wall						\
	-O0 						\
	-g 							\
	-c							\
	-D_RTE_						\
	-DM55_HE

CLANG_CFLAGS += 				\
	$(OPTIMIZATION)				\
	$(GENERATE_DEBUG_SYMBOLS)	\
	-mcpu=cortex-m55 			\
	-mfpu=none 					\
	-mfloat-abi=soft 			\
	-mthumb 					\
	-mlittle-endian				\
	-xc 						\
	-std=c99 					\
	-MD 						\
	-MP 						\
	-c							\
	-DTEST_DEBUG 				\
	-DCC_SB_SUPPORT_IOT 		\
	-D_RTE_						\
	$(INCLUDES)					
CSUPEROPT = -Os # save some code space for performance-critical code

# Select debugging or optimisation build.
#ifeq ($(DEBUG), 1)
#CFLAGS += -Og
#else
#CFLAGS += -Os -DNDEBUG
#CFLAGS += -fdata-sections -ffunction-sections
#endif

# Set linker flags.
#LDFLAGS += -nostdlib -T stm32f405.ld --gc-sections
LDFLAGS += -T m55_he_services_test.ld
LDFLAGS += -specs=nosys.specs
LDFLAGS += -march=armv8.1-m.main
LDFLAGS	+= -Wl,-Map=nano.map,--cref
LDFLAGS	+= -Wl,--gc-sections
LDFLAGS += -Xlinker -print-memory-usage -Xlinker

# Define the required source files.
#SRC_C += lib.c main.c system.c
SRC_C += main.c
SRC_C +=												 \
	$(DEVICE_SRC_DIR)/mpu_M55_HE.c					     \
	$(DEVICE_SRC_DIR)/startup_M55_HE.c				     \
	$(DEVICE_SRC_DIR)/system_M55_HE.c				     \
	$(DEVICE_SRC_DIR)/system_utils.c				     \
	$(DEVICE_SRC_DIR)/tgu_M55_HE.c

# Define the required object files.
OBJ += $(PY_CORE_O)
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))

# Define the top-level target, the main firmware.
all: $(BUILD)/firmware.dfu

$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
	$(Q)$(CC) $(LDFLAGS) -o $@ $^
	$(Q)$(SIZE) $@

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(ECHO) "Create $@"
	$(Q)$(OBJCOPY) -O binary -j .isr_vector -j .text -j .data $^ $@

$(BUILD)/firmware.dfu: $(BUILD)/firmware.bin
	$(ECHO) "Create $@"
	$(Q)$(PYTHON) $(DFU) -b 0x08000000:$^ $@

deploy: $(BUILD)/firmware.dfu
	$(Q)$(PYTHON) $(PYDFU) -u $^

# Include remaining core make rules.
include $(TOP)/py/mkrules.mk
