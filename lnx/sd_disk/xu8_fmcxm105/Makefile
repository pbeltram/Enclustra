# Makefile for SD binaries

override SRC_TOP:=..

XILSRC=~/opt/Xilinx/Vitis/2020.2/settings64.sh

.PHONY : all help build_lx build_boot clean distclean cpbuild_lx cpbuild_boot

all: help

help:
	@echo "usage: make [target]"
	@echo
	@echo "  target:"
	@echo "         help             # This info."
	@echo "         clean            # Delete build directory files."
	@echo "         distclean        # Delete all generated files."
	@echo "         build_lx         # Builds bin/image.ub using rootfs.its file."
	@echo "         build_boot       # Builds bin/BOOT.BIN using boot.bif file."
	@echo
	@echo "         cpbuild_lx       # Writes bin/image.ub to SD boot partition."
	@echo "         cpbuild_boot     # Writes bin/BOOT.BIN to SD boot partition."
	@echo
	@echo "  Examples:"
	@echo "         make build_lx # Builds bin/image.ub."
	@echo "         make cpbuild_lx # Writes bin/image.ub to SD boot partition."
	@echo
	@echo "         make build_boot # Builds bin/BOOT.BIN."
	@echo "         make cpbuild_boot # Writes bin/BOOT.BIN to SD boot partition."
	@echo

build_lx: clean
	@echo "==> Building image.ub"
	-mkdir -p ./build
	-mkdir -p ./bin
	cp ./Image.gz ./build
	cp ./system.dtb ./build
	mkimage -f ./rootfs.its ./bin/image.ub
	md5sum ./bin/image.ub

build_boot: clean
	@echo "==> Building BOOT.BIN"
	-mkdir -p ./build
	-mkdir -p ./bin
	cp ./zynqmp_fsbl.elf ./build
	cp ./pmufw.elf ./build
	cp ./system.bit ./build
	cp ./bl31.elf ./build
	cp ./u-boot.elf ./build
	source $(XILSRC); bootgen -arch zynqmp -image ./boot.bif -w on -o ./bin/BOOT.BIN -log trace
	md5sum ./bin/BOOT.BIN

cpbuild_lx:
	scp ./bin/image.ub root@xu8:/media/sd-mmcblk1p1/
	
cpbuild_boot:
	scp ./bin/BOOT.BIN root@xu8:/media/sd-mmcblk1p1/
	scp ./uboot.scr root@xu8:/media/sd-mmcblk1p1/

clean:
	-rm -rf build

distclean: clean
	-rm -rf bin

