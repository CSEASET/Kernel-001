# Makefile for the Kernel-001 src

CC=gcc
NASM=nasm
LINKER=ld

C_FLAGS=-m32
NASM_FLAGS=-f elf32
LD_FLAGS=-m elf_i386

SOURCES=kernel.c

all: 
	$(CC) $(C_FLAGS) -c $(SOURCES) -o kernel.o
	$(NASM) $(NASM_FLAGS) loader.asm -o kasm.o
	$(LINKER) $(LD_FLAGS) -T link.ld -o ../build/kernel kasm.o kernel.o
	rm *.o
