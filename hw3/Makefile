# Compiler and flags
CC = gcc
CFLAGS = -ffreestanding -O0 -g -Wall -Wextra -c
LDFLAGS = -ffreestanding -O0 -g -Wall -Wextra

# Output kernel binary
TARGET = kernel.bin

# Source files
SRCS = kernel_main.c page.c
OBJS = $(SRCS:.c=.o)

# Default target: build the kernel
all: $(TARGET)

# Link the kernel binary
$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

# Compile C source files
%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJS) $(TARGET)

# Optional: run in QEMU (if you have it installed)
run: $(TARGET)
	qemu-system-x86_64 -kernel $(TARGET) -serial stdio -m 512M

.PHONY: all clean run
