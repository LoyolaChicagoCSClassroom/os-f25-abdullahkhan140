CC=gcc
CFLAGS=-ffreestanding -O0 -g -Wall -Wextra -m32
OBJ=multiboot_header.o kernel_main.o page.o interrupt.o keyboard.o rprintf.o terminal.o fat.o

all: kernel.bin

# Compile C files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile assembly files (if any)
%.o: %.s
	$(CC) $(CFLAGS) -c $< -o $@

# Link kernel
kernel.bin: $(OBJ)
	$(CC) $(CFLAGS) -o kernel.bin $(OBJ)

# Clean up
clean:
	rm -f *.o kernel.bin

# Run in QEMU
run: kernel.bin
	qemu-system-i386 -kernel kernel.bin -serial stdio -m 512M

