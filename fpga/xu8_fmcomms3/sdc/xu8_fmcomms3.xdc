# clocks
create_clock -name rx_clk       -period  4.00 [get_ports RX_CLK_IN_P]

# Define SPI clock
create_clock -name spi0_clk     -period 40   [get_pins -hier */EMIOSPI0SCLKO]
