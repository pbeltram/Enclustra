# Devicetree Makefile

override SRC_TOP:=..

BUILD?=xu8_fmcxm105

CCROSS?_COMPILE?=aarch64-linux-gnu-
LX_DIR?=../linux/linux-rpt

DEVICETREE=system.dtb

CPP=$(CROSS_COMPILE)cpp
DTC=dtc

INSTALL_DIR ?= .

SRC_FILES=$(wildcard $(BUILD)/*.dtsi $(BUILD)/*.dts $(BUILD)/include/*)
SRC_MAIN=$(BUILD)/system-top.dts

.PHONY : all install clean distclean

all: $(SRC_FILES) $(DEVICETREE)

$(DEVICETREE): $(SRC_FILES)
	@echo "==> Building device tree for $(BUILD)"
	$(CPP) -nostdinc -I ./$(BUILD)/. -I ./$(BUILD)/include -I $(LX_DIR)/include -undef -x assembler-with-cpp $(SRC_MAIN) $(SRC_MAIN).preprocessed
	$(DTC) -I dts -O dtb -o $@ $(SRC_MAIN).preprocessed

clean:
	-rm -f $(DEVICETREE)
	-rm -f $(SRC_MAIN).preprocessed
	-rm -f *.jou *.log *.preprocessed

install: $(DEVICETREE)
	cp $(DEVICETREE) $(INSTALL_DIR)

