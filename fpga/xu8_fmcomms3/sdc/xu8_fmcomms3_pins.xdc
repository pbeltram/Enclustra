###############################################################################
## Copyright (C) 2016-2023 Analog Devices, Inc. All rights reserved.
### SPDX short identifier: ADIBSD
###############################################################################

# constraints
# ad9361

set_property  -dict {PACKAGE_PIN  AA8   IOSTANDARD LVDS DIFF_TERM_ADV TERM_100} [get_ports RX_CLK_IN_P]     ; ## G06  FMC_HPC0_LA00_CC_P	IO_B66_L12_GC_AA8_P
set_property  -dict {PACKAGE_PIN  AB8   IOSTANDARD LVDS DIFF_TERM_ADV TERM_100} [get_ports RX_CLK_IN_N]     ; ## G07  FMC_HPC0_LA00_CC_N	IO_B66_L12_GC_AB8_N
set_property  -dict {PACKAGE_PIN  AB6   IOSTANDARD LVDS DIFF_TERM_ADV TERM_100} [get_ports RX_FRAME_IN_P]   ; ## D08  FMC_HPC0_LA01_CC_P	IO_B66_L14_GC_AB6_P
set_property  -dict {PACKAGE_PIN  AB5   IOSTANDARD LVDS DIFF_TERM_ADV TERM_100} [get_ports RX_FRAME_IN_N]   ; ## D09  FMC_HPC0_LA01_CC_N	IO_B66_L14_GC_AB5_N
set_property  -dict {PACKAGE_PIN  AB10  IOSTANDARD LVDS DIFF_TERM_ADV TERM_100} [get_ports RX_DATA_IN_P[0]] ; ## H07  FMC_HPC0_LA02_P	IO_B66_L5_AD14_AB10_P
set_property  -dict {PACKAGE_PIN  AB9   IOSTANDARD LVDS DIFF_TERM_ADV TERM_100} [get_ports RX_DATA_IN_N[0]] ; ## H08  FMC_HPC0_LA02_N	IO_B66_L5_AD14_AB9_N
set_property  -dict {PACKAGE_PIN  AC9   IOSTANDARD LVDS DIFF_TERM_ADV TERM_100} [get_ports RX_DATA_IN_P[1]] ; ## G09  FMC_HPC0_LA03_P	IO_B66_L7_AD13_AC9_P
set_property  -dict {PACKAGE_PIN  AD9   IOSTANDARD LVDS DIFF_TERM_ADV TERM_100} [get_ports RX_DATA_IN_N[1]] ; ## G10  FMC_HPC0_LA03_N	IO_B66_L7_AD13_AD9_N
set_property  -dict {PACKAGE_PIN  W8    IOSTANDARD LVDS DIFF_TERM_ADV TERM_100} [get_ports RX_DATA_IN_P[2]] ; ## H10  FMC_HPC0_LA04_P	IO_B66_L9_AD12_W8_P
set_property  -dict {PACKAGE_PIN  Y8    IOSTANDARD LVDS DIFF_TERM_ADV TERM_100} [get_ports RX_DATA_IN_N[2]] ; ## H11  FMC_HPC0_LA04_N	IO_B66_L9_AD12_Y8_N
set_property  -dict {PACKAGE_PIN  Y7    IOSTANDARD LVDS DIFF_TERM_ADV TERM_100} [get_ports RX_DATA_IN_P[3]] ; ## D11  FMC_HPC0_LA05_P	IO_B66_L10_AD4_Y7_P
set_property  -dict {PACKAGE_PIN  AA7   IOSTANDARD LVDS DIFF_TERM_ADV TERM_100} [get_ports RX_DATA_IN_N[3]] ; ## D12  FMC_HPC0_LA05_N	IO_B66_L10_AD4_AA7_N
set_property  -dict {PACKAGE_PIN  AC6   IOSTANDARD LVDS DIFF_TERM_ADV TERM_100} [get_ports RX_DATA_IN_P[4]] ; ## C10  FMC_HPC0_LA06_P	IO_B66_L8_AD5_AC6_P
set_property  -dict {PACKAGE_PIN  AD6   IOSTANDARD LVDS DIFF_TERM_ADV TERM_100} [get_ports RX_DATA_IN_N[4]] ; ## C11  FMC_HPC0_LA06_N	IO_B66_L8_AD5_AD6_N
set_property  -dict {PACKAGE_PIN  AA6   IOSTANDARD LVDS DIFF_TERM_ADV TERM_100} [get_ports RX_DATA_IN_P[5]] ; ## H13  FMC_HPC0_LA07_P	IO_B66_L18_AD2_AA6_P
set_property  -dict {PACKAGE_PIN  AA5   IOSTANDARD LVDS DIFF_TERM_ADV TERM_100} [get_ports RX_DATA_IN_N[5]] ; ## H14  FMC_HPC0_LA07_N	IO_B66_L18_AD2_AA5_N

set_property  -dict {PACKAGE_PIN  AD5   IOSTANDARD LVDS} [get_ports TX_CLK_OUT_P]                           ; ## G12  FMC_HPC0_LA08_P	IO_B66_L15_AD11_AD5_P
set_property  -dict {PACKAGE_PIN  AE5   IOSTANDARD LVDS} [get_ports TX_CLK_OUT_N]                           ; ## G13  FMC_HPC0_LA08_N	IO_B66_L15_AD11_AE5_N
set_property  -dict {PACKAGE_PIN  AB4   IOSTANDARD LVDS} [get_ports TX_FRAME_OUT_P]                         ; ## D14  FMC_HPC0_LA09_P	IO_B66_L17_AD10_AB4_P
set_property  -dict {PACKAGE_PIN  AC4   IOSTANDARD LVDS} [get_ports TX_FRAME_OUT_N]                         ; ## D15  FMC_HPC0_LA09_N	IO_B66_L17_AD10_AC4_N
set_property  -dict {PACKAGE_PIN  AC3   IOSTANDARD LVDS} [get_ports TX_DATA_OUT_P[0]]                       ; ## H16  FMC_HPC0_LA11_P	IO_B66_L21_AD8_AC3_P
set_property  -dict {PACKAGE_PIN  AC2   IOSTANDARD LVDS} [get_ports TX_DATA_OUT_N[0]]                       ; ## H17  FMC_HPC0_LA11_N	IO_B66_L21_AD8_AC2_N
set_property  -dict {PACKAGE_PIN  AA3   IOSTANDARD LVDS} [get_ports TX_DATA_OUT_P[1]]                       ; ## G15  FMC_HPC0_LA12_P	IO_B66_L22_AD0_AA3_P
set_property  -dict {PACKAGE_PIN  AB3   IOSTANDARD LVDS} [get_ports TX_DATA_OUT_N[1]]                       ; ## G16  FMC_HPC0_LA12_N	IO_B66_L22_AD0_AB3_N
set_property  -dict {PACKAGE_PIN  AD2   IOSTANDARD LVDS} [get_ports TX_DATA_OUT_P[2]]                       ; ## D17  FMC_HPC0_LA13_P	IO_B66_L19_AD9_AD2_P
set_property  -dict {PACKAGE_PIN  AE2   IOSTANDARD LVDS} [get_ports TX_DATA_OUT_N[2]]                       ; ## D18  FMC_HPC0_LA13_N	IO_B66_L19_AD9_AE2_N
set_property  -dict {PACKAGE_PIN  AD4   IOSTANDARD LVDS} [get_ports TX_DATA_OUT_P[3]]                       ; ## C14  FMC_HPC0_LA10_P	IO_B66_L16_AD3_AD4_P
set_property  -dict {PACKAGE_PIN  AE4   IOSTANDARD LVDS} [get_ports TX_DATA_OUT_N[3]]                       ; ## C15  FMC_HPC0_LA10_N	IO_B66_L16_AD3_AE4_N
set_property  -dict {PACKAGE_PIN  AA2   IOSTANDARD LVDS} [get_ports TX_DATA_OUT_P[4]]                       ; ## C18  FMC_HPC0_LA14_P	IO_B66_L24_AA2_P
set_property  -dict {PACKAGE_PIN  AA1   IOSTANDARD LVDS} [get_ports TX_DATA_OUT_N[4]]                       ; ## C19  FMC_HPC0_LA14_N	IO_B66_L24_AA1_N
set_property  -dict {PACKAGE_PIN  AD1   IOSTANDARD LVDS} [get_ports TX_DATA_OUT_P[5]]                       ; ## H19  FMC_HPC0_LA15_P	IO_B66_L20_AD1_AD1_P
set_property  -dict {PACKAGE_PIN  AE1   IOSTANDARD LVDS} [get_ports TX_DATA_OUT_N[5]]                       ; ## H20  FMC_HPC0_LA15_N	IO_B66_L20_AD1_AE1_N

set_property  -dict {PACKAGE_PIN  AB1   IOSTANDARD LVCMOS18} [get_ports ENABLE]                             ; ## G18  FMC_HPC0_LA16_P	IO_B66_L23_AB1_P
set_property  -dict {PACKAGE_PIN  AC1   IOSTANDARD LVCMOS18} [get_ports TXNRX]                              ; ## G19  FMC_HPC0_LA16_N	IO_B66_L23_AC1_N
set_property  -dict {PACKAGE_PIN  AB11  IOSTANDARD LVCMOS18} [get_ports GPIO_STATUS[0]]                     ; ## G21  FMC_HPC0_LA20_P	IO_B66_L3_AD15_AB11_P
set_property  -dict {PACKAGE_PIN  AC11  IOSTANDARD LVCMOS18} [get_ports GPIO_STATUS[1]]                     ; ## G22  FMC_HPC0_LA20_N	IO_B66_L3_AD15_AC11_N
set_property  -dict {PACKAGE_PIN  AA12  IOSTANDARD LVCMOS18} [get_ports GPIO_STATUS[2]]                     ; ## H25  FMC_HPC0_LA21_P	IO_B66_L2_AA12_P
set_property  -dict {PACKAGE_PIN  AA11  IOSTANDARD LVCMOS18} [get_ports GPIO_STATUS[3]]                     ; ## H26  FMC_HPC0_LA21_N	IO_B66_L2_AA11_N
set_property  -dict {PACKAGE_PIN  Y10   IOSTANDARD LVCMOS18} [get_ports GPIO_STATUS[4]]                     ; ## G24  FMC_HPC0_LA22_P	IO_B66_L6_AD6_Y10_P
set_property  -dict {PACKAGE_PIN  AA10  IOSTANDARD LVCMOS18} [get_ports GPIO_STATUS[5]]                     ; ## G25  FMC_HPC0_LA22_N	IO_B66_L6_AD6_AA10_N
set_property  -dict {PACKAGE_PIN  AD11  IOSTANDARD LVCMOS18} [get_ports GPIO_STATUS[6]]                     ; ## D23  FMC_HPC0_LA23_P	IO_B66_L4_AD7_AD11_P
set_property  -dict {PACKAGE_PIN  AD10  IOSTANDARD LVCMOS18} [get_ports GPIO_STATUS[7]]                     ; ## D24  FMC_HPC0_LA23_N	IO_B66_L4_AD7_AD10_N
set_property  -dict {PACKAGE_PIN  AE3   IOSTANDARD LVCMOS18} [get_ports GPIO_CTL[0]]                        ; ## H28  FMC_HPC0_LA24_P	IO_B66_AE3
set_property  -dict {PACKAGE_PIN  Y1    IOSTANDARD LVCMOS18} [get_ports GPIO_CTL[1]]                        ; ## H29  FMC_HPC0_LA24_N	IO_B66_Y1
#set_property  -dict {PACKAGE_PIN XXXX  IOSTANDARD LVCMOS18} [get_ports GPIO_CTL[2]]                        ; ## G27  FMC_HPC0_LA25_P XXXX
#set_property  -dict {PACKAGE_PIN XXXX  IOSTANDARD LVCMOS18} [get_ports GPIO_CTL[3]]                        ; ## G28  FMC_HPC0_LA25_N XXXX
set_property  -dict {PACKAGE_PIN  AC12  IOSTANDARD LVCMOS18} [get_ports GPIO_EN_AGC]                        ; ## H22  FMC_HPC0_LA19_P IO_B66_L1_AC12_P
set_property  -dict {PACKAGE_PIN  AD12  IOSTANDARD LVCMOS18} [get_ports GPIO_SYNC]                          ; ## H23  FMC_HPC0_LA19_N	IO_B66_L1_AD12_N
#set_property  -dict {PACKAGE_PIN XXXX  IOSTANDARD LVCMOS18} [get_ports GPIO_RESETB]                        ; ## H31  FMC_HPC0_LA28_P XXXX

set_property -dict {PACKAGE_PIN E13   IOSTANDARD LVCMOS18  }                [get_ports {SPI_CSN}]  ; #IO0_06 IO0_D23_N IO_BN_L7_HDGC_AD5_E13_N ## C27  FMC_HPC0_LA27_N XXXX
set_property -dict {PACKAGE_PIN B16   IOSTANDARD LVCMOS18  }                [get_ports {SPI_CLK}]  ; #IO0_08 IO0_D21_N IO_BO_L10_AD10_B16_N    ## C26  FMC_HPC0_LA27_P XXXX
set_property -dict {PACKAGE_PIN G14   IOSTANDARD LVCMOS18  PULLTYPE PULLUP} [get_ports {SPI_MISO}] ; #IO0_10 IO0_D19_N IO_BN_L4_AD8_G14_N      ## D27  FMC_HPC0_LA26_N XXXX
set_property -dict {PACKAGE_PIN J12   IOSTANDARD LVCMOS18  }                [get_ports {SPI_MOSI}] ; #IO0_12 IO0_D17_N IO_BN_L2_AD10_J12_N     ## D26  FMC_HPC0_LA26_P XXXX

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

