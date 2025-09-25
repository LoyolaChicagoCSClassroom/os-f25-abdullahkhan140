UNAME_M := $(shell uname -m)

ifeq ($(UNAME_M),aarch64)
PREFIX:=i386-unknown-elf-
BOOTIMG:=/usr/local/grub/lib/grub/i386-pc/boot.img
GRUBLOC:=/usr/local/grub/bin/
else
PREFIX:=
BOOTIMG:=/usr/lib/grub/i386-pc/boot.img
GRUBLOC :=
endif

CC := $(PREFIX)gcc
LD := $(PREFIX)ld
OBJCOPY := $(PREFIX)objcopy
SIZE := $(PREFIX)size

CFLAGS := -ffreestanding -mgeneral-regs-only -mno-mmx -m32 -march=i386 -fno-pie -fno-stack-protector -g3 -Wall -Isrc

ODIR = obj
SDIR = src

OBJS = kernel_main.o rprintf.o

OBJ = $(patsubst %,$(ODIR)/%,$(OBJS))

$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) $(CFLAGS) -c -g -o $@ $^

all: bin

bin: obj $(OBJ)
	$(LD) -melf_i386 $(OBJ) -Tkernel.ld -o kernel
	$(SIZE) kernel

obj:
	mkdir -p obj

run: bin
	qemu-system-i386 -kernel kernel -m 32M

clean:
	rm -f obj/* kernel
