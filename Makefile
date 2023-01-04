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
ALIF_DEVICE=Ensemble
ALIF_CMSIS_VERSION=0.4.1
CMSIS_VERSION=5.9.0
ifneq ($(HOST_OS), LINUX_OS)
USER_BASE    := $(subst \,/,$(USERPROFILE))
INCLUDE_BASE := $(USER_BASE)/AppData/Local/Arm/Packs/ARM/CMSIS/$(CMSIS_VERSION)
ALIF_PACK_SOURCE := $(USER_BASE)/AppData/Local/Arm/Packs/AlifSemiconductor
else
USER_BASE    := /home/$(shell whoami)
INCLUDE_BASE := $(USER_BASE)/.cache/arm/packs/ARM/CMSIS/$(CMSIS_VERSION)
ALIF_PACK_SOURCE := $(USER_BASE)/.cache/arm/packs/AlifSemiconductor
endif
ALIF_BASE    := $(ALIF_PACK_SOURCE)/$(ALIF_DEVICE)/$(ALIF_CMSIS_VERSION)

# ARM and ALIF CMSIS 
CFLAGS += -I./RTE
CFLAGS += -I$(INCLUDE_BASE)/CMSIS/Core/Include/CMSIS/Core/Include
CFLAGS += -I$(INCLUDE_BASE)/CMSIS/Core/Include
CFLAGS += -I$(INCLUDE_BASE)/CMSIS/Driver/Include
CFLAGS += -I$(INCLUDE_BASE)/Device/ARM/ARMCM55/Include
CFLAGS += -I$(ALIF_BASE)/CMSIS/Driver/Inc
CFLAGS += -I$(ALIF_BASE)/CMSIS/Include
CFLAGS += -I$(ALIF_BASE)/Device/M55_HE/Include
CFLAGS += -I$(ALIF_BASE)/Device/M55_HE/Config
CFLAGS += -I$(ALIF_BASE)/Device/M55_HE

# Set CFLAGS.
CFLAGS += -I. -I$(TOP) -I$(BUILD) 
CFLAGS += $(INCLUDES)
CFLAGS += 						\
	-march=armv8.1-m.main		\
	-Wall						\
	-O0 						\
	-g 							\
	-c							\
	-D_RTE_						\
	-DM55_HE					\
	-DCPU=CPU_M55_HE

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
LDFLAGS += -specs=nosys.specs
#LDFLAGS += -nostdlib 
LDFLAGS	+= -T m55_he_services_test.ld 
LDFLAGS += -march=armv8.1-m.main
LDFLAGS	+= -lm
LDFLAGS	+= -Wl,-Map=$(BUILD)/firmware.map,--cref
LDFLAGS	+= -Wl,--gc-sections
LDFLAGS += -Xlinker -print-memory-usage -Xlinker

# Porting layer extras
#SRC_C += 												\
#    shared/runtime/gchelper_generic.c 					\
#    shared/runtime/pyexec.c 							\
#    shared/runtime/stdout_helpers.c 					\

# ALIF boot straps and CMSIS bits
SRC_C += main.c 
SRC_C += newlib_stubs.c
SRC_C += retarget.c
SRC_C += uart_tracelib.c
SRC_C += Driver_USART.c 
SRC_C += Driver_PINMUX_AND_PINPAD.c
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

install:	$(BUILD)/firmware.elf
	$(CP) m55-upython.json ../../../../firmware/setools/app-release-exec/build/config
	$(CP) $(BUILD)/firmware.bin ../../../../firmware/setools/app-release-exec/build/images

$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
	@$(CC) $(LDFLAGS) $(OBJ) -o $(BUILD)/firmware.elf 
	$(Q)$(SIZE) $@

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(ECHO) "Create $@"
#	$(Q)$(OBJCOPY) -O binary -j .isr_vector -j .text -j .data $^ $@
	$(ECHO) 'Generate binary file' $(BUILD)/firmware.bin
	$(OBJCOPY) -O binary $(BUILD)/firmware.elf $(BUILD)/firmware.bin

$(BUILD)/firmware.dfu: $(BUILD)/firmware.bin
	$(ECHO) "Create $@"
	$(Q)$(PYTHON) $(DFU) -b 0x08000000:$^ $@

deploy: $(BUILD)/firmware.dfu
	$(Q)$(PYTHON) $(PYDFU) -u $^

# Include remaining core make rules.
include $(TOP)/py/mkrules.mk
