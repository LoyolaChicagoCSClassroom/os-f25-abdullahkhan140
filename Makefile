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
	# Create a 32 MB empty disk image
	dd if=/dev/zero of=rootfs.img bs=1M count=32

	# Format as plain FAT16 (no offset)
	mkfs.vfat rootfs.img

	# Create /boot directory in the image
	mmd -i rootfs.img ::/boot

	# Copy kernel and grub.cfg into the image
	mcopy -i rootfs.img kernel ::/
	mcopy -i rootfs.img grub.cfg ::/boot

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

# Run kernel in QEMU (depends on rootfs.img)
run: rootfs.img
	qemu-system-i386 -drive file=rootfs.img,format=raw -m 512 -serial stdio

debug:
	./launch_qemu.sh

clean:
	rm -f grub.img kernel rootfs.img $(ISO) obj/*
