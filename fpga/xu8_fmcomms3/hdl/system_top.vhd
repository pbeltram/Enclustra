
--------------------------------------------------------------------------------
-- libraries
--------------------------------------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

library unisim;
use unisim.vcomponents.all;

--------------------------------------------------------------------------------
-- entity declaration
--------------------------------------------------------------------------------
entity system_top is
  port (
    RX_CLK_IN_N   : in  std_logic;
    RX_CLK_IN_P   : in  std_logic;
    RX_DATA_IN_N  : in  std_logic_vector(6-1 downto 0);
    RX_DATA_IN_P  : in  std_logic_vector(6-1 downto 0);
    RX_FRAME_IN_N : in  std_logic;
    RX_FRAME_IN_P : in  std_logic;
  
    TX_CLK_OUT_N   : out std_logic;
    TX_CLK_OUT_P   : out std_logic;
    TX_DATA_OUT_N  : out std_logic_vector(6-1 downto 0);
    TX_DATA_OUT_P  : out std_logic_vector(6-1 downto 0);
    TX_FRAME_OUT_N : out std_logic;
    TX_FRAME_OUT_P : out std_logic;

    ENABLE : out std_logic;
    TXNRX  : out std_logic;
    
    GPIO_STATUS : in  std_logic_vector(8-1 downto 0);
    GPIO_CTL    : out std_logic_vector(2-1 downto 0); -- NOTE: GPIO_CTL[2], GPIO_CTL[2] not connected.
    GPIO_EN_AGC : out std_logic;
    GPIO_SYNC   : out std_logic;
--  GPIO_RESETB : out std_logic; NOTE: Not connected.

    SPI_MISO : in  std_logic;
    SPI_MOSI : out std_logic;
    SPI_CSN  : out std_logic;
    SPI_CLK  : out std_logic;

    -- Display Port
    DP_HPD     : in  std_logic;
    DP_AUX_IN  : in  std_logic;
    DP_AUX_OE  : out std_logic;
    DP_AUX_OUT : out std_logic;
    
    -- I2C FPGA
    I2C_SCL_FPGA : inout std_logic;
    I2C_SDA_FPGA : inout std_logic;
    
    BTN1_N    : in  std_logic; -- ST1 BUTTONS
    PL_LED2_N : out std_logic; -- XU8 LED
    LED2      : out std_logic; -- ST1 LED
    LED3      : out std_logic  -- ST1 LED
  );
end system_top;

--------------------------------------------------------------------------------
-- Architecture section
--------------------------------------------------------------------------------
architecture rtl of system_top is

--------------------------------------------------------------------------------
-- Constant Declarations
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
-- Signals declaration
--------------------------------------------------------------------------------
signal dp_aux_data_oe_n : std_logic;

signal btn1 : std_logic;

signal spi0_csn : std_logic_vector(3-1 downto 0);

signal tdd_sync     : std_logic;
signal gpio_ctl_out : std_logic_vector(4-1 downto 0);

--------------------------------------------------------------------------------
-- Implementation
--------------------------------------------------------------------------------
begin
--------------------------------------------------------------------------------
-- Signal connections
--------------------------------------------------------------------------------
DP_AUX_OE <= not dp_aux_data_oe_n;

btn1 <= not BTN1_N;

LED2 <= '0'; --TODO:  LED asserted when hi.
LED3 <= '0'; --TODO:  LED asserted when hi.

SPI_CSN <= spi0_csn(0);

tdd_sync <= '1';
GPIO_CTL <= gpio_ctl_out(1 downto 0);

--------------------------------------------------------------------------------
-- Processes
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
-- Modules
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
system_wrapper_i: entity work.system_wrapper
  port map
  (
    I2C_FPGA_scl_io => I2C_SCL_FPGA,
    I2C_FPGA_sda_io => I2C_SDA_FPGA,

    PL_LED2_N => PL_LED2_N,

    DP_AUX_OUT => DP_AUX_OUT,
    DP_AUX_OE  => dp_aux_data_oe_n,
    DP_AUX_IN  => DP_AUX_IN,
    DP_HPD     => DP_HPD,

    SPI0_CSN  => spi0_csn ,
    SPI0_MISO => SPI_MISO ,
    SPI0_MOSI => SPI_MOSI ,
    SPI0_SCLK => SPI_CLK  ,
    
    RX_CLK_IN_N   => RX_CLK_IN_N   ,
    RX_CLK_IN_P   => RX_CLK_IN_P   ,
    RX_DATA_IN_N  => RX_DATA_IN_N  ,
    RX_DATA_IN_P  => RX_DATA_IN_P  ,
    RX_FRAME_IN_N => RX_FRAME_IN_N ,
    RX_FRAME_IN_P => RX_FRAME_IN_P ,

    TX_CLK_OUT_N   => TX_CLK_OUT_N   ,
    TX_CLK_OUT_P   => TX_CLK_OUT_P   ,
    TX_DATA_OUT_N  => TX_DATA_OUT_N  ,
    TX_DATA_OUT_P  => TX_DATA_OUT_P  ,
    TX_FRAME_OUT_N => TX_FRAME_OUT_N ,
    TX_FRAME_OUT_P => TX_FRAME_OUT_P ,

    ENABLE => ENABLE ,
    TXNRX  => TXNRX  ,
    
    GPIO_STATUS => GPIO_STATUS  ,
    GPIO_CTL    => gpio_ctl_out ,
    GPIO_EN_AGC => GPIO_EN_AGC  ,
    GPIO_SYNC   => GPIO_SYNC    ,
    GPIO_RESETB => open         ,

    TDD_SYNC_I => tdd_sync ,
    TDD_SYNC_O => open ,
    TDD_SYNC_T => open
  );
--------------------------------------------------------------------------------
  
end rtl;
