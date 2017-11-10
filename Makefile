MEMBASEADDR=0x000265E8

SILENT=SILENT

NOZERO100K=TRUE

##########################################################################
##########################################################################
##########################################################################

ifndef OSTYPE
  HOSTPLATFORM:=$(shell uname -s | head -c 5)
  ifeq ($(HOSTPLATFORM),MINGW)
    OSTYPE = Windows
    EXE = .exe
    SH = sh
    DEVNULL = NUL
    SORT = $(dir $(shell which uniq.exe | sed s/\\\\/\\\//g))/sort.exe
  else
    ifeq ($(HOSTPLATFORM),Linux)
      OSTYPE = Linux
      EXE =
      SH =
      DEVNULL = /dev/null
      SORT = sort
    else
      OSTYPE = Other
      EXE =
      SH =
      DEVNULL = /dev/null
      SORT = sort
    endif
  endif
else
  EXE =
  SH =
  DEVNULL = /dev/null
  SORT = sort
endif

##########################################################################
##########################################################################
##########################################################################

HOSTCC=gcc
HOSTCFLAGS=-g -O2 -Wall

PAKWIF=$(topdir)tools/pakwif$(EXE)

##########################################################################

export PATH+=:/c:/gcc/bin

CC=arm-elf-gcc
OBJCOPY=arm-elf-objcopy
OBJDUMP=arm-elf-objdump
STRIP=arm-elf-strip
NM=arm-elf-nm
AR=arm-elf-ar
RANLIB=arm-elf-ranlib
SIZE=arm-elf-size

# -msoft-float is used per default but object files are marked as hard-float
CFLAGS=-fno-inline -Os -fno-strict-aliasing -march=armv5t 
# flags for gcc v4
# CFLAGS+=-mtune=arm946e-s
CTHUMB=-mthumb
CFLAGS+=-mthumb-interwork
CFLAGS+=-Wall -Wno-unused -Wno-format -W--use-blx

##########################################################################

default: all

ifndef SKIPBUILDRULES

%.o: %.c
	@echo $< \-\> $@
	$(CC) $(CFLAGS) -nostdinc -c -o $@ $<
%.o: %.S
	@echo $< \-\> $@
	$(CC) $(CFLAGS) -nostdinc -c -o $@ $<
%.a:
	@echo $^ \-\> $@
	$(AR) rcs $@ $^

.dep/%.d: %.c .dep
	$(CC) $(CFLAGS) -M $< > $@.$$$$; \
	    sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	    rm -f $@.$$$$

.dep/%.d: %.S .dep
	$(CC) $(CFLAGS) -M $< > $@.$$$$; \
	    sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	    rm -f $@.$$$$

endif



LDLIBS=
LDOPTS=-nostdlib -Wl,--allow-shlib-undefined -Wl,--no-define-common,-EL,-T,link-boot.ld
LDOPTS+=-Wl,-N,-Ttext,$(MEMBASEADDR)

CPPFLAGS+=-I.

CORE_FILE=$(topdir)main.bin

all: main.bin

OBJS=entry.o main.o

main.bin: main.elf
	@echo $< \-\> $@
	$(OBJDUMP) -z -d main.elf > main.dump
	$(OBJCOPY) -O binary main.elf main.bin

main.elf: $(OBJS)
	@echo \-\> $@
	$(CC) $(CFLAGS) -o $@ $^   $(LDLIBS) $(LDFLAGS) $(LDOPTS)
	( $(NM) $@ | grep ' U ' > $@.syms ) && exit 1 || exit 0
clean: 
	rm -f $(OBJS) main.bin main.elf main.dump main.elf.syms
