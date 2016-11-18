# Makefile
#

### start of configuration area

TARGET = main
SRC = main.c battery.c serial.c  init.c # SoftI2CMaster/SoftI2CMaster.h
CXXSRC =
ASRC= 
# uno or diecemlia
UNO = y

ifeq ($(UNO),y)
PORT = /dev/ttyACM0
else
PORT = /dev/ttyUSB0
endif

### end of configuration area

ifeq ($(UNO),y)
MCU = atmega328p
else
MCU = atmega168
endif
F_CPU = 16000000
FORMAT = ihex
UPLOAD_RATE = 19200

# Name of this Makefile (used for "make depend").
MAKEFILE = Makefile

# Debugging format.
# Native formats for AVR-GCC's -g are stabs [default], or dwarf-2.
# AVR (extended) COFF requires stabs, plus an avr-objcopy run.
DEBUG = stabs
OPT = s
#main.o: OPT = 0

# Place -D or -U options here
CDEFS = -DF_CPU=$(F_CPU)
CXXDEFS = -DF_CPU=$(F_CPU)

# Place -I options here
CINCS = # -I$(ARDUINO)
CXXINCS = # -I$(ARDUINO)

CSTANDARD = # -std=gnu99
CDEBUG = -g$(DEBUG)
CWARN = -Wall -Wstrict-prototypes
CTUNING = -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
#CEXTRA = -Wa,-adhlns=$(<:.c=.lst)

CFLAGS = $(CDEBUG) $(CDEFS) $(CINCS) -O$(OPT) $(CWARN) $(CSTANDARD) $(CEXTRA) $(CTUNING)
CXXFLAGS = $(CDEFS) $(CINCS) -O$(OPT)
ASFLAGS = -Wa,-adhlns=$(<:.S=.lst),-gstabs 
LDFLAGS = # -L$(ARDUINO) -lcore

# Programming support using avrdude. Settings and variables.
ifeq ($(UNO),y)
AVRDUDE_MCU = m328p
AVRDUDE_PROGRAMMER = arduino
else
AVRDUDE_MCU = atmega168
AVRDUDE_PROGRAMMER = stk500v1
AVRDUDE_UPLOAD = -b $(UPLOAD_RATE)
endif
AVRDUDE_PORT = $(PORT)
AVRDUDE_WRITE_FLASH = -U flash:w:$(TARGET).hex
AVRDUDE_FLAGS = -F -p $(AVRDUDE_MCU) -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER) $(AVRDUDE_UPLOAD)

# Program settings
CC = avr-gcc
CXX = avr-g++
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE = avr-size
NM = avr-nm
AVRDUDE = avrdude
REMOVE = /bin/rm -f
MV = mv -f
SUDO = sudo
FUSER=/bin/fuser
SH  = /bin/sh

# Define all object files.
OBJ = $(SRC:.c=.o) $(CXXSRC:.cpp=.o) $(ASRC:.S=.o)

# Define all listing files.
LST = $(ASRC:.S=.lst) $(CXXSRC:.cpp=.lst) $(SRC:.c=.lst)

# Combine all necessary flags and optional flags.
# Add target processor to flags.
ALL_CFLAGS = -mmcu=$(MCU) -I. $(CFLAGS)
ALL_CXXFLAGS = -mmcu=$(MCU) -I. $(CXXFLAGS)
ALL_ASFLAGS = -mmcu=$(MCU) -I. -x assembler-with-cpp $(ASFLAGS)

# Default target.
#all: core build 
all: build

build: elf hex lst

elf: $(TARGET).elf
hex: $(TARGET).hex
eep: $(TARGET).eep
lst: $(TARGET).lst 
sym: $(TARGET).sym

core:
	make -C $(ARDUINO) libcore.a

# Program the device.  
upload: $(TARGET).hex
	$(SUDO) $(SH) -c 'p="$$(lsof -t $(PORT))" && kill -9 $$p ; true'
	$(SUDO) $(FUSER) -k -9 /dev/ttyUSB0 2>/dev/null ; true
	$(SUDO) $(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH)

.SUFFIXES: .elf .hex .lst .sym

.elf.hex:
	$(OBJCOPY) -O $(FORMAT) -R .eeprom $< $@

# Create extended listing file from ELF output file.
.elf.lst:
	$(OBJDUMP) -h -S $< > $@
	@$(SIZE) $(TARGET).elf

# Create a symbol table from ELF output file.
.elf.sym:
	$(NM) -n $< > $@

# Link: create ELF output file from object files.
$(TARGET).elf: $(OBJ)
	$(CC) -mmcu=atmega168 $(OBJ) --output $@ $(LDFLAGS)

# Compile: create object files from C++ source files.
.cpp.o:
	$(CXX) -c $(ALL_CXXFLAGS) $< -o $@ 

# Compile: create object files from C source files.
.c.o:
	$(CC) -c $(ALL_CFLAGS) $< -o $@ 

# Compile: create assembler files from C source files.
.c.s:
	$(CC) -S $(ALL_CFLAGS) $< -o $@

# Assemble: create object files from assembler source files.
.S.o:
	$(CC) -c $(ALL_ASFLAGS) $< -o $@

# Target: clean project.
clean:
	$(REMOVE) $(TARGET).hex $(TARGET).eep $(TARGET).cof $(TARGET).elf \
	$(TARGET).map $(TARGET).sym $(TARGET).lst \
	$(OBJ) $(LST) $(SRC:.c=.s) $(SRC:.c=.d) $(CXXSRC:.cpp=.s) $(CXXSRC:.cpp=.d)

depend:
	if grep '^# DO NOT DELETE' $(MAKEFILE) >/dev/null; \
	then \
		sed -e '/^# DO NOT DELETE/,$$d' $(MAKEFILE) > \
			$(MAKEFILE).$$$$ && \
		$(MV) $(MAKEFILE).$$$$ $(MAKEFILE); \
	fi
	echo '# DO NOT DELETE THIS LINE -- make depend depends on it.' \
		>> $(MAKEFILE); \
	$(CC) -M -mmcu=$(MCU) $(ALL_CFLAGS) $(SRC) $(ASRC) >> $(MAKEFILE)

.PHONY:	all build elf hex eep lst sym program coff extcoff clean depend core

# DO NOT DELETE THIS LINE -- make depend depends on it.
