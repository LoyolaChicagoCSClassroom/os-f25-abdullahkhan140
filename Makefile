UNAME_M := $(shell uname -m)

ifeq ($(UNAME_M),aarch64)
PREFIX := i386-unknown-elf-
BOOTIMG := /usr/local/grub/lib/grub/i386-pc/boot.img
GRUBLOC := /usr/local/grub/bin/
else
PREFIX :=
BOOTIMG := /usr/lib/grub/i386-pc/boot.img
GRUBLOC :=
endif

CC := $(PREFIX)gcc
LD := $(PREFIX)ld
SIZE := $(PREFIX)size

CFLAGS := -ffreestanding -fno-exceptions -fno-asynchronous-unwind-tables \
          -mgeneral-regs-only -m32 -march=i386 -fno-pie -fno-stack-protector -g3 -Wall -Isrc

ODIR = obj
SDIR = src

OBJS = kernel_main.o rprintf.o
OBJ = $(patsubst %,$(ODIR)/%,$(OBJS))

all: bin

bin: $(ODIR) $(OBJ)
	$(LD) -melf_i386 $(OBJ) -T kernel.ld -o kernel
	$(SIZE) kernel

$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(ODIR):
	mkdir -p $(ODIR)

clean:
	rm -rf $(ODIR) kernel
