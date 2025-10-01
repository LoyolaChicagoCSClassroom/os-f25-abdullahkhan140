# Ubuntu Makefile for simple OS class project

CC       := gcc
LD       := ld
OBJDUMP  := objdump
OBJCOPY  := objcopy
SIZE     := size

CONFIGS  := -DCONFIG_HEAP_SIZE=4096
CFLAGS   := -ffreestanding -m32 -march=i386 -fno-pie -fno-stack-protector -g3 -Wall -Wextra

ODIR     := obj
SDIR     := src

OBJS = \
	kernel_main.o \

OBJ = $(patsubst %,$(ODIR)/%,$(OBJS))

BOOTIMG  := /usr/lib/grub/i386-pc/boot.img
GRUBLOC  := /usr/bin/

# --------------------------------------------------------------------

$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $^

$(ODIR)/%.o: $(SDIR)/%.s
	$(CC) $(CFLAGS) -c -o $@ $^

all: bin rootfs.img

# Build kernel binary
bin: obj $(OBJ)
	$(LD) -m elf_i386 obj/* -Tkernel.ld -o kernel
	$(SIZE) kernel

obj:
	mkdir -p obj

# Build bootable root filesystem image
rootfs.img:
	dd if=/dev/zero of=rootfs.img bs=1M count=32
	grub-mkimage -p "(hd0,msdos1)/boot" -o grub.img -O i386-pc normal biosdisk multiboot multiboot2 configfile fat part_msdos
	dd if=$(BOOTIMG) of=rootfs.img conv=notrunc
	echo 'start=2048, type=83, bootable' | sfdisk rootfs.img
	mkfs.vfat --offset 2048 -F16 rootfs.img
	mcopy -i rootfs.img@@1M kernel ::/
	mmd -i rootfs.img@@1M boot
	mcopy -i rootfs.img@@1M grub.cfg ::/boot
	@echo " -- BUILD COMPLETED SUCCESSFULLY --"

# Build ISO (optional, in case sfdisk/mtools not available)
ISO_DIR := iso_tmp
ISO     := grub.iso

.PHONY: iso run_iso

iso: bin $(ISO)

$(ISO): kernel grub.cfg
	rm -f $(ISO)
	rm -rf $(ISO_DIR)
	mkdir -p $(ISO_DIR)/boot/grub
	cp kernel $(ISO_DIR)/
	cp grub.cfg $(ISO_DIR)/boot/grub/
	grub-mkrescue -o $(ISO) $(ISO_DIR)
	@if [ ! -f $(ISO) ]; then \
		echo "ERROR: grub-mkrescue failed to create $(ISO)" >&2; \
		exit 1; \
	fi

run_iso: $(ISO)
	qemu-system-i386 -cdrom $(ISO) -serial stdio

# Run kernel in QEMU
run:
	qemu-system-i386 -hda rootfs.img -serial stdio

debug:
	./launch_qemu.sh

clean:
	rm -f grub.img kernel rootfs.img $(ISO) obj/*
