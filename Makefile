# Directories
ODIR := obj
SDIR := src

# Files
OBJS := kernel_main.o rprintf.o
OBJ := $(patsubst %,$(ODIR)/%,$(OBJS))

# Tools
CC := gcc
LD := ld
GRUB_MKRESCUE := grub-mkrescue
SIZE := size

# Flags
CFLAGS := -ffreestanding -m32 -g -Wall -Isrc

# Targets
all: kernel.iso

# Build object files
$(ODIR)/%.o: $(SDIR)/%.c | $(ODIR)
	$(CC) $(CFLAGS) -c -o $@ $<

# Create obj dir
$(ODIR):
	mkdir -p $(ODIR)

# Link kernel
kernel.bin: $(OBJ)
	$(LD) -m elf_i386 -T kernel.ld -o kernel.bin $(OBJ)
	$(SIZE) kernel.bin

# Make ISO with GRUB
kernel.iso: kernel.bin
	mkdir -p iso/boot/grub
	cp kernel.bin iso/boot/kernel.bin
	cat > iso/boot/grub/grub.cfg <<EOF
set timeout=0
set default=0

menuentry "My Kernel" {
    multiboot /boot/kernel.bin
    boot
}
EOF
	grub-mkrescue -o kernel.iso iso

# Clean
clean:
	rm -rf $(ODIR) kernel.bin iso
