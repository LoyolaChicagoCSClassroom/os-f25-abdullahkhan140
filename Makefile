# =========================================================
# Bare-metal i386 OS Makefile
# =========================================================

CC := gcc
LD := ld
SIZE := size
OBJCOPY := objcopy

CFLAGS := -ffreestanding -fno-exceptions -fno-asynchronous-unwind-tables \
          -m32 -march=i386 -fno-pie -fno-stack-protector -g3 -Wall -Isrc

ODIR := obj
SDIR := src
ISO_DIR := iso

OBJS := kernel_main.o rprintf.o
OBJ := $(patsubst %,$(ODIR)/%,$(OBJS))

KERNEL := kernel
ISO := kernel.iso

all: $(KERNEL)

$(KERNEL): $(ODIR) $(OBJ) kernel.ld
	$(LD) -m elf_i386 $(OBJ) -T kernel.ld -o $(KERNEL)
	$(SIZE) $(KERNEL)

$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(ODIR):
	mkdir -p $(ODIR)

clean:
	rm -rf $(ODIR) $(KERNEL) $(ISO) $(ISO_DIR)

iso: $(KERNEL)
	mkdir -p $(ISO_DIR)/boot/grub
	cp $(KERNEL) $(ISO_DIR)/boot/
	echo 'set timeout=0
set default=0

menuentry "My Kernel" {
    multiboot /boot/$(KERNEL)
    boot
}' > $(ISO_DIR)/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO) $(ISO_DIR)

run: iso
	qemu-system-i386 -cdrom $(ISO) -m 32M
