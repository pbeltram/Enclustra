# ----------------------------------------------------------------------------------
# Copyright (c) 2022 by Enclustra GmbH, Switzerland.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of
# this hardware, software, firmware, and associated documentation files (the
# "Product"), to deal in the Product without restriction, including without
# limitation the rights to use, copy, modify, merge, publish, distribute,
# sublicense, and/or sell copies of the Product, and to permit persons to whom the
# Product is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Product.
#
# THE PRODUCT IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
# PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
# HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
# OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
# PRODUCT OR THE USE OR OTHER DEALINGS IN THE PRODUCT.
# ----------------------------------------------------------------------------------

set_property BITSTREAM.CONFIG.OVERTEMPSHUTDOWN ENABLE [current_design]
 
# ----------------------------------------------------------------------------------
# Important! Do not remove this constraint!
# This property ensures that all unused pins are set to high impedance.
# If the constraint is removed, all unused pins have to be set to HiZ in the top level file.
set_property BITSTREAM.CONFIG.UNUSEDPIN PULLNONE [current_design]
# ----------------------------------------------------------------------------------

# FMC HPC Connector
set_property -dict {PACKAGE_PIN AA8  IOSTANDARD LVCMOS18 } [get_ports {CONN_LVDS_P[0]}]  ; #FMC_G6   IO_B66_L12_GC_AA8_P    FMC_LA00_CC_P  NOTE: Global clock! J1_1
set_property -dict {PACKAGE_PIN AB8  IOSTANDARD LVCMOS18 } [get_ports {CONN_LVDS_N[0]}]  ; #FMC_G7   IO_B66_L12_GC_AB8_N    FMC_LA00_CC_N  NOTE: Global clock! J1_3
set_property -dict {PACKAGE_PIN AB6  IOSTANDARD LVCMOS18 } [get_ports {CONN_LVDS_P[1]}]  ; #FMC_D8   IO_B66_L14_GC_AB6_P    FMC_LA01_CC_P  NOTE: Global clock! J1_5
set_property -dict {PACKAGE_PIN AB5  IOSTANDARD LVCMOS18 } [get_ports {CONN_LVDS_N[1]}]  ; #FMC_D9   IO_B66_L14_GC_AB5_N    FMC_LA01_CC_N  NOTE: Global clock! J1_7
set_property -dict {PACKAGE_PIN AB10 IOSTANDARD LVCMOS18 } [get_ports {CONN_LVDS_P[2]}]  ; #FMC_H7   IO_B66_L5_AD14_AB10_P  FMC_LA02_P J1_9
set_property -dict {PACKAGE_PIN AB9  IOSTANDARD LVCMOS18 } [get_ports {CONN_LVDS_N[2]}]  ; #FMC_H8   IO_B66_L5_AD14_AB9_N   FMC_LA02_N J1_11
set_property -dict {PACKAGE_PIN AC9  IOSTANDARD LVCMOS18 } [get_ports {CONN_LVDS_P[3]}]  ; #FMC_G9   IO_B66_L7_AD13_AC9_P   FMC_LA03_P J1_13
set_property -dict {PACKAGE_PIN AD9  IOSTANDARD LVCMOS18 } [get_ports {CONN_LVDS_N[3]}]  ; #FMC_G10  IO_B66_L7_AD13_AD9_N   FMC_LA03_N J1_15
set_property -dict {PACKAGE_PIN W8   IOSTANDARD LVCMOS18 } [get_ports {CONN_LVDS_P[4]}]  ; #FMC_H10  IO_B66_L9_AD12_W8_P    FMC_LA04_P J1_17
set_property -dict {PACKAGE_PIN Y8   IOSTANDARD LVCMOS18 } [get_ports {CONN_LVDS_N[4]}]  ; #FMC_H11  IO_B66_L9_AD12_Y8_N    FMC_LA04_N J1_19
set_property -dict {PACKAGE_PIN Y7   IOSTANDARD LVCMOS18 } [get_ports {CONN_LVDS_P[5]}]  ; #FMC_D11  IO_B66_L10_AD4_Y7_P    FMC_LA05_P J1_21
set_property -dict {PACKAGE_PIN AA7  IOSTANDARD LVCMOS18 } [get_ports {CONN_LVDS_N[5]}]  ; #FMC_D12  IO_B66_L10_AD4_AA7_N   FMC_LA05_N J1_23
set_property -dict {PACKAGE_PIN AC6  IOSTANDARD LVCMOS18 } [get_ports {CONN_LVDS_P[6]}]  ; #FMC_C10  IO_B66_L8_AD5_AC6_P    FMC_LA06_P J1_25
set_property -dict {PACKAGE_PIN AD6  IOSTANDARD LVCMOS18 } [get_ports {CONN_LVDS_N[6]}]  ; #FMC_C11  IO_B66_L8_AD5_AD6_N    FMC_LA06_N J1_27
set_property -dict {PACKAGE_PIN AA6  IOSTANDARD LVCMOS18 } [get_ports {CONN_LVDS_P[7]}]  ; #FMC_H13  IO_B66_L18_AD2_AA6_P   FMC_LA07_P J1_29
set_property -dict {PACKAGE_PIN AA5  IOSTANDARD LVCMOS18 } [get_ports {CONN_LVDS_N[7]}]  ; #FMC_H14  IO_B66_L18_AD2_AA5_N   FMC_LA07_N J1_31
set_property -dict {PACKAGE_PIN AD5  IOSTANDARD LVCMOS18 } [get_ports {CONN_LVDS_P[8]}]  ; #FMC_G12  IO_B66_L15_AD11_AD5_P  FMC_LA08_P J1_33
set_property -dict {PACKAGE_PIN AE5  IOSTANDARD LVCMOS18 } [get_ports {CONN_LVDS_N[8]}]  ; #FMC_G13  IO_B66_L15_AD11_AE5_N  FMC_LA08_N J1_35
set_property -dict {PACKAGE_PIN AB4  IOSTANDARD LVCMOS18 } [get_ports {CONN_LVDS_P[9]}]  ; #FMC_D14  IO_B66_L17_AD10_AB4_P  FMC_LA09_P J1_37
set_property -dict {PACKAGE_PIN AC4  IOSTANDARD LVCMOS18 } [get_ports {CONN_LVDS_N[9]}]  ; #FMC_D15  IO_B66_L17_AD10_AC4_N  FMC_LA09_N J1_39
set_property -dict {PACKAGE_PIN AD4  IOSTANDARD LVCMOS18 } [get_ports {CONN_LVDS_P[10]}] ; #FMC_C14  IO_B66_L16_AD3_AD4_P   FMC_LA10_P J1_2
set_property -dict {PACKAGE_PIN AE4  IOSTANDARD LVCMOS18 } [get_ports {CONN_LVDS_N[10]}] ; #FMC_C15  IO_B66_L16_AD3_AE4_N   FMC_LA10_N J1_4
set_property -dict {PACKAGE_PIN AC3  IOSTANDARD LVCMOS18 } [get_ports {CONN_LVDS_P[11]}] ; #FMC_H16  IO_B66_L21_AD8_AC3_P   FMC_LA11_P J1_6
set_property -dict {PACKAGE_PIN AC2  IOSTANDARD LVCMOS18 } [get_ports {CONN_LVDS_N[11]}] ; #FMC_H17  IO_B66_L21_AD8_AC2_N   FMC_LA11_N J1_8
set_property -dict {PACKAGE_PIN AA3  IOSTANDARD LVCMOS18 } [get_ports {CONN_LVDS_P[12]}] ; #FMC_G15  IO_B66_L22_AD0_AA3_P   FMC_LA12_P J1_10
set_property -dict {PACKAGE_PIN AB3  IOSTANDARD LVCMOS18 } [get_ports {CONN_LVDS_N[12]}] ; #FMC_G16  IO_B66_L22_AD0_AB3_N   FMC_LA12_N J1_12
set_property -dict {PACKAGE_PIN AD2  IOSTANDARD LVCMOS18 } [get_ports {CONN_LVDS_P[13]}] ; #FMC_D17  IO_B66_L19_AD9_AD2_P   FMC_LA13_P J1_14
set_property -dict {PACKAGE_PIN AE2  IOSTANDARD LVCMOS18 } [get_ports {CONN_LVDS_N[13]}] ; #FMC_D18  IO_B66_L19_AD9_AE2_N   FMC_LA13_N J1_16
set_property -dict {PACKAGE_PIN AA2  IOSTANDARD LVCMOS18 } [get_ports {CONN_LVDS_P[14]}] ; #FMC_C18  IO_B66_L24_AA2_P       FMC_LA14_P J1_18
set_property -dict {PACKAGE_PIN AA1  IOSTANDARD LVCMOS18 } [get_ports {CONN_LVDS_N[14]}] ; #FMC_C19  IO_B66_L24_AA1_N       FMC_LA14_N J1_20
set_property -dict {PACKAGE_PIN AD1  IOSTANDARD LVCMOS18 } [get_ports {CONN_LVDS_P[15]}] ; #FMC_H19  IO_B66_L20_AD1_AD1_P   FMC_LA15_P J1_22
set_property -dict {PACKAGE_PIN AE1  IOSTANDARD LVCMOS18 } [get_ports {CONN_LVDS_N[15]}] ; #FMC_H20  IO_B66_L20_AD1_AE1_N   FMC_LA15_N J1_24
set_property -dict {PACKAGE_PIN AB1  IOSTANDARD LVCMOS18 } [get_ports {CONN_LVDS_P[16]}] ; #FMC_G19  IO_B66_L23_AB1_P       FMC_LA16_P J1_26
set_property -dict {PACKAGE_PIN AC1  IOSTANDARD LVCMOS18 } [get_ports {CONN_LVDS_N[16]}] ; #FMC_G18  IO_B66_L23_AC1_N       FMC_LA16_N J1_28
set_property -dict {PACKAGE_PIN AD7  IOSTANDARD LVCMOS18 } [get_ports {CONN_LVDS_P[17]}] ; #FMC_D21  IO_B66_L13_GC_AD7_P    FMC_LA17_CC_P  NOTE: Global clock! J1_30
set_property -dict {PACKAGE_PIN AE7  IOSTANDARD LVCMOS18 } [get_ports {CONN_LVDS_N[17]}] ; #FMC_D20  IO_B66_L13_GC_AE7_N    FMC_LA17_CC_N  NOTE: Global clock! J1_32
set_property -dict {PACKAGE_PIN AB11 IOSTANDARD LVCMOS18 } [get_ports {CONN_LVDS_P[18]}] ; #FMC_G21  IO_B66_L3_AD15_AB11_P  FMC_LA20_P J20_1
set_property -dict {PACKAGE_PIN AC11 IOSTANDARD LVCMOS18 } [get_ports {CONN_LVDS_N[18]}] ; #FMC_G22  IO_B66_L3_AD15_AC11_N  FMC_LA20_N J20_3
set_property -dict {PACKAGE_PIN AA12 IOSTANDARD LVCMOS18 } [get_ports {CONN_LVDS_P[19]}] ; #FMC_H25  IO_B66_L2_AA12_P       FMC_LA21_P J20_5
set_property -dict {PACKAGE_PIN AA11 IOSTANDARD LVCMOS18 } [get_ports {CONN_LVDS_N[19]}] ; #FMC_H26  IO_B66_L2_AA11_N       FMC_LA21_N J20_7
set_property -dict {PACKAGE_PIN Y10  IOSTANDARD LVCMOS18 } [get_ports {CONN_LVDS_P[20]}] ; #FMC_G24  IO_B66_L6_AD6_Y10_P    FMC_LA22_P J20_9
set_property -dict {PACKAGE_PIN AA10 IOSTANDARD LVCMOS18 } [get_ports {CONN_LVDS_N[20]}] ; #FMC_G25  IO_B66_L6_AD6_AA10_N   FMC_LA22_N J20_11
set_property -dict {PACKAGE_PIN AD11 IOSTANDARD LVCMOS18 } [get_ports {CONN_LVDS_P[21]}] ; #FMC_D32  IO_B66_L4_AD7_AD11_P   FMC_LA23_P J20_13
set_property -dict {PACKAGE_PIN AD10 IOSTANDARD LVCMOS18 } [get_ports {CONN_LVDS_N[21]}] ; #FMC_D24  IO_B66_L4_AD7_AD10_N   FMC_LA23_N J20_15
set_property -dict {PACKAGE_PIN AE3  IOSTANDARD LVCMOS18 } [get_ports {CONN_LVDS_P[22]}] ; #FMC_H28  IO_B66_AE3             FMC_LA24_P J20_2
set_property -dict {PACKAGE_PIN Y1   IOSTANDARD LVCMOS18 } [get_ports {CONN_LVDS_N[22]}] ; #FMC_H29  IO_B66_Y1              FMC_LA24_N J20_4

set_property -dict {PACKAGE_PIN AC12 IOSTANDARD LVCMOS18 } [get_ports {CONN_J1_38_P}] ; #FMC_H22  IO_B66_L1_AC12_P       FMC_LA19_P J1_38
set_property -dict {PACKAGE_PIN AD12 IOSTANDARD LVCMOS18 } [get_ports {CONN_J1_40_N}] ; #FMC_H23  IO_B66_L1_AD12_N       FMC_LA19_N J1_40

set_property -dict {PACKAGE_PIN AC7  IOSTANDARD LVCMOS18 } [get_ports {SI570_CLK_N}]  ; #FMC_H5   IO_B66_L11_GC_AC7_N   FMC_CLK0_M2C_N   NOTE: FMCXM105 Si570 osc.
set_property -dict {PACKAGE_PIN AC8  IOSTANDARD LVCMOS18 } [get_ports {SI570_CLK_P}]  ; #FMC_H4   IO_B66_L11_GC_AC8_P   FMC_CLK0_M2C_P   NOTE: FMCXM105 Si570 osc.

set_property -dict {PACKAGE_PIN V2 } [get_ports {MGT_BA_RX0_P}] ; #FMC_K13  MGT_BA_RX0_V2_P      FMC_HA10_P
set_property -dict {PACKAGE_PIN V1 } [get_ports {MGT_BA_RX0_N}] ; #FMC_K14  MGT_BA_RX0_V1_N      FMC_HA10_N
set_property -dict {PACKAGE_PIN W4 } [get_ports {MGT_BA_TX0_P}] ; #FMC_J12  MGT_BA_TX0_W4_P      FMC_HA11_P
set_property -dict {PACKAGE_PIN W3 } [get_ports {MGT_BA_TX0_N}] ; #FMC_J13  MGT_BA_TX0_W3_N      FMC_HA11_N

set_property -dict {PACKAGE_PIN U4 } [get_ports {MGT_BA_RX1_P}] ; #FMC_F10  MGT_BA_RX1_U4_P      FMC_HA08_P
set_property -dict {PACKAGE_PIN U3 } [get_ports {MGT_BA_RX1_N}] ; #FMC_F11  MGT_BA_RX1_U3_N      FMC_HA08_N
set_property -dict {PACKAGE_PIN V6 } [get_ports {MGT_BA_TX1_P}] ; #FMC_E9   MGT_BA_TX1_V6_P      FMC_HA09_P
set_property -dict {PACKAGE_PIN V5 } [get_ports {MGT_BA_TX1_N}] ; #FMC_E10  MGT_BA_TX1_V5_N      FMC_HA09_N

set_property -dict {PACKAGE_PIN T2 } [get_ports {MGT_BA_RX2_P}] ; #FMC_F4   MGT_BA_RX2_T2_P      FMC_HA00_CC_P  NOTE: Not clk!
set_property -dict {PACKAGE_PIN T1 } [get_ports {MGT_BA_RX2_N}] ; #FMC_F5   MGT_BA_RX2_T1_N      FMC_HA00_CC_N  NOTE: Not clk!
set_property -dict {PACKAGE_PIN T6 } [get_ports {MGT_BA_TX2_P}] ; #FMC_J9   MGT_BA_TX2_T6_P      FMC_HA07_P
set_property -dict {PACKAGE_PIN T5 } [get_ports {MGT_BA_TX2_N}] ; #FMC_J10  MGT_BA_TX2_T5_N      FMC_HA07_N

set_property -dict {PACKAGE_PIN P2 } [get_ports {MGT_BA_RX3_P}] ; #FMC_K10  MGT_BA_RX3_P2_P      FMC_HA06_P
set_property -dict {PACKAGE_PIN P1 } [get_ports {MGT_BA_RX3_N}] ; #FMC_K11  MGT_BA_RX3_P1_N      FMC_HA06_N
set_property -dict {PACKAGE_PIN R4 } [get_ports {MGT_BA_TX3_P}] ; #FMC_E2   MGT_BA_TX3_R4_P      FMC_HA01_CC_P  NOTE: Not clk!
set_property -dict {PACKAGE_PIN R3 } [get_ports {MGT_BA_TX3_N}] ; #FMC_E3   MGT_BA_TX3_R3_N      FMC_HA01_CC_N  NOTE: Not clk!

# Display Port
set_property -dict {PACKAGE_PIN AG19 IOSTANDARD LVCMOS12  } [get_ports {DP_HPD}]
set_property -dict {PACKAGE_PIN AF17 IOSTANDARD LVCMOS12  } [get_ports {DP_AUX_IN}]
set_property -dict {PACKAGE_PIN AH16 IOSTANDARD LVCMOS12  } [get_ports {DP_AUX_OE}]
set_property -dict {PACKAGE_PIN AC19 IOSTANDARD LVCMOS12  } [get_ports {DP_AUX_OUT}]

# I2C FPGA
set_property -dict {PACKAGE_PIN K15  IOSTANDARD LVCMOS18  } [get_ports {I2C_SCL_FPGA}]
set_property -dict {PACKAGE_PIN K14  IOSTANDARD LVCMOS18  } [get_ports {I2C_SDA_FPGA}]

# BUTTONS
set_property -dict {PACKAGE_PIN K12  IOSTANDARD LVCMOS18  } [get_ports {BTN1_N}]     ; #BTN1   IO_BN_L1_AD11_K12_P   BTN1_N
# Mercury_XU8 LED
set_property -dict {PACKAGE_PIN AF13 IOSTANDARD LVCMOS12  } [get_ports {PL_LED2_N}]  ; #D202   IO_T1U12_64_AF13      PL_LED2_N  NOTE: XU8 D202 LED.
# ST1 LED
set_property -dict {PACKAGE_PIN AK9  IOSTANDARD LVCMOS12  } [get_ports {LED2}]       ; #D1606  IO_B65_L7_AD13_AK9_P  LED2
set_property -dict {PACKAGE_PIN AK8  IOSTANDARD LVCMOS12  } [get_ports {LED3}]       ; #D1607  IO_B65_L7_AD13_AK8_N  LED3

# ST1 Xtal Oscillator 100MHz
set_property -dict {PACKAGE_PIN K11  IOSTANDARD LVCMOS18  } [get_ports {CLK_100_CAL}]      ; #  IO_BN_L1_AD11_K11_N      CLK_100_CAL  NOTE: ST1 100Mhz osc.
# Mercury_XU8 Xtal 100MHz Oscillator
set_property -dict {PACKAGE_PIN AJ6  IOSTANDARD DIFF_SSTL12_DCI} [get_ports {CLK100_PL_N}] ; #  IO_L13N_T2L0Q_GC_AJ6_N   CLK100_PL_N  NOTE: XU8 100Mhz osc.
set_property -dict {PACKAGE_PIN AH6  IOSTANDARD DIFF_SSTL12_DCI} [get_ports {CLK100_PL_P}] ; #  IO_L13P_T2L0Q_GC_AH6_P   CLK100_PL_P  NOTE: XU8 100Mhz osc.
# SI5338B Clock Generator
set_property -dict {PACKAGE_PIN C13 } [get_ports {CLK_USR_N}]           ; #  IO_BN_L8_HDGC_AD4_C13_N  CLK_USR_N    NOTE: Si5338B CLK0B 125.0MHz
set_property -dict {PACKAGE_PIN D14 } [get_ports {CLK_USR_P}]           ; #  IO_BN_L8_HDGC_AD4_D14_P  CLK_USR_P    NOTE: Si5338B CLK0A 125.0MHz
set_property -dict {PACKAGE_PIN B9  } [get_ports {CLK_REF0_N}]          ; #  MGT_BD_REFCLK1_B9_N      CLK_REF0_N   NOTE: Si5338B CLK1B 156.250MHz
set_property -dict {PACKAGE_PIN B10 } [get_ports {CLK_REF0_P}]          ; #  MGT_BD_REFCLK1_B10_P     CLK_REF0_P   NOTE: Si5338B CLK1A 156.250MHz
set_property -dict {PACKAGE_PIN F9  } [get_ports {CLK_REF1_N}]          ; #  MGT_BC_REFCLK1_F9_N      CLK_REF1_N   NOTE: Si5338B CLK2B 156.250MHz
set_property -dict {PACKAGE_PIN F10 } [get_ports {CLK_REF1_P}]          ; #  MGT_BC_REFCLK1_F10_P     CLK_REF1_P   NOTE: Si5338B CLK2A 156.250MHz
set_property -dict {PACKAGE_PIN L7  } [get_ports {CLK_REF2_N}]          ; #  MGT_BB_REFCLK0_L7_N      CLK_REF2_N   NOTE: Si5338B CLK3B 156.250Mhz
set_property -dict {PACKAGE_PIN L8  } [get_ports {CLK_REF2_P}]          ; #  MGT_BB_REFCLK0_L8_P      CLK_REF2_P   NOTE: Si5338B CLK3A 156.250Mhz

