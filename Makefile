UNAME_M := $(shell uname -m)

ifeq ($(UNAME_M),aarch64)
PREFIX := i686-linux-gnu-
BOOTIMG := /usr/lib/grub/i386-pc/boot.img
GRUBLOC := /usr/lib/grub/i386-pc/
else
PREFIX :=
BOOTIMG := /usr/lib/grub/i386-pc/boot.img
GRUBLOC := /usr/lib/grub/i386-pc/
endif

CC := $(PREFIX)gcc
LD := $(PREFIX)ld
OBJDUMP := $(PREFIX)objdump
OBJCOPY := $(PREFIX)objcopy
SIZE := $(PREFIX)size
CONFIGS := -DCONFIG_HEAP_SIZE=4096
CFLAGS := -ffreestanding -mgeneral-regs-only -mno-mmx -m32 -march=i386 -fno-pie -fno-stack-protector -g3 -Wall

ODIR = obj
SDIR = src

OBJS := \
	rprintf.o \
	terminal.o \
	kernel_main.o

OBJ = $(patsubst %,$(ODIR)/%,$(OBJS))

$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) $(CFLAGS) -c -g -o $@ $^

$(ODIR)/%.o: $(SDIR)/%.s
	$(CC) $(CFLAGS) -c -g -o $@ $^

all: bin rootfs.img

bin: obj $(OBJ)
	$(LD) -m elf_i386 obj/* -Tkernel.ld -o kernel
	$(SIZE) kernel

obj:
	mkdir -p obj

rootfs.img:
	dd if=/dev/zero of=rootfs.img bs=1M count=32
	$(GRUBLOC)grub-mkimage -p "(hd0,msdos1)/boot" -o grub.img -O i386-pc normal biosdisk multiboot multiboot2 configfile fat exfat part_msdos
	dd if=$(BOOTIMG) of=rootfs.img conv=notrunc
	dd if=grub.img of=rootfs.img conv=notrunc seek=1
	# Create a single partition
	echo "label: dos" | sudo sfdisk rootfs.img
	# Make FAT16 filesystem starting from sector 2048
	sudo mkfs.vfat -F16 -n ROOTFS -C rootfs.img 32768
	# Copy kernel and grub.cfg
	mmd -i rootfs.img ::/boot
	mcopy -i rootfs.img kernel ::/
	mcopy -i rootfs.img grub.cfg ::/boot/

debug:
	./launch_qemu.sh

clean:
	rm -f grub.img kernel rootfs.img obj/*

.PHONY: run
run: all
	@if command -v qemu-system-i386 >/dev/null 2>&1; then \
		qemu-system-i386 -m 256 -drive file=$(PWD)/rootfs.img,format=raw,if=ide -boot c -display curses; \
	else \
		qemu-system-x86_64 -cpu qemu32 -m 256 -drive file=$(PWD)/rootfs.img,format=raw,if=ide -boot c -display curses; \
	fi
