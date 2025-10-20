# Compiler and flags
CC = gcc
CFLAGS = -ffreestanding -O0 -g -Wall -Wextra -c
LDFLAGS = -ffreestanding -O0 -g -Wall -Wextra

# Output binary
TARGET = kernel.bin

# Object files
OBJS = kernel_main.o page.o

# Default target
all: $(TARGET)

# Link the kernel
$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

# Compile kernel_main.c
kernel_main.o: kernel_main.c page.h
	$(CC) $(CFLAGS) kernel_main.c -o kernel_main.o

# Compile page.c
page.o: page.c page.h
	$(CC) $(CFLAGS) page.c -o page.o

# Clean build files
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
