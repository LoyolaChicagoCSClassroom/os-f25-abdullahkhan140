# =========================================================
# Makefile for i386 Bare-Metal OS
# =========================================================

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

# =========================================================
# Default target
all: kernel

# =========================================================
# Build kernel
kernel: $(ODIR) $(OBJ)
	$(LD) -melf_i386 $(OBJ) -T kernel.ld -o kernel
	$(SIZE) kernel

# =========================================================
# Compile C files
$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

# =========================================================
# Create object directory
$(ODIR):
	mkdir -p $(ODIR)

# =========================================================
# Clean
clean:
	rm -rf $(ODIR) kernel rootfs.img grub.img

# =========================================================
# Build a bootable image
rootfs.img: kernel
	dd if=/dev/zero of=rootfs.img bs=1M count=32
	$(GRUBLOC)grub-mkimage -p "(hd0,msdos1)/boot" -o grub.img -O i386-pc normal biosdisk multiboot multiboot2 configfile fat exfat part_msdos
	dd if=$(BOOTIMG) of=rootfs.img conv=notrunc
	dd if=grub.img of=rootfs.img conv=notrunc bs=512 seek=1
	mkfs.vfat --offset 2048 -F16 rootfs.img
	mcopy -i rootfs.img@@1M kernel ::/
	mmd -i rootfs.img@@1M boot 
	mcopy -i rootfs.img@@1M grub.cfg ::/boot
	@echo " -- ROOTFS IMAGE BUILT -- "

# =========================================================
# Run kernel in QEMU
run: rootfs.img
	qemu-system-i386 -hda rootfs.img -m 32M -boot order=c

