# Makefile for the Kernel-001

SRC_DIR=src

all: install
	echo -e '\n\n BUILD COMPLETED!!1\n Use "make bochs" or "make qemu" to run this build with an emulator.'

install:
	$(MAKE) -C $(SRC_DIR) --always-make
	cp ./build/kernel ./build/iso/boot/kernel-001
	grub-mkrescue -o ./build/bootable.iso ./build/iso

bochs: install
	cd ./build && bochs -f bochsrc.txt


qemu: install
	qemu-system-x86_64 -hda ./build/bootable.iso
