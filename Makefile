
CC = gcc
CFLAGS = -ffreestanding -m32 -O0 -g -Wall
LDFLAGS = -T kernel.ld -m elf_i386

# Directories
SRC_DIR = src
OBJ_DIR = obj

# Source files
SRC = $(SRC_DIR)/kernel_main.c \
      $(SRC_DIR)/rprintf.c \
      $(SRC_DIR)/interrupt.c \
      $(SRC_DIR)/page.c

# Object files
OBJ = $(OBJ_DIR)/kernel_main.o \
      $(OBJ_DIR)/rprintf.o \
      $(OBJ_DI_
