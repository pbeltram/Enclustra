
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
    
    -- Oscillator 100 MHz
    CLK_100_CAL                    : in    std_logic;
    -- PL 100 MHz Oscillator
    CLK100_PL_N                    : in    std_logic;
    CLK100_PL_P                    : in    std_logic;
    -- Clock Generator
    CLK_USR_N                      : in    std_logic;
    CLK_USR_P                      : in    std_logic;
    CLK_REF0_N                     : in    std_logic;
    CLK_REF0_P                     : in    std_logic;
    CLK_REF1_N                     : in    std_logic;
    CLK_REF1_P                     : in    std_logic;
    CLK_REF2_N                     : in    std_logic;
    CLK_REF2_P                     : in    std_logic;
    -- FMCXM105 Si570
    SI570_CLK_N                    : in    std_logic;
    SI570_CLK_P                    : in    std_logic;
    
    -- Display Port
    DP_HPD                         : in    std_logic;
    DP_AUX_IN                      : in    std_logic;
    DP_AUX_OE                      : out   std_logic;
    DP_AUX_OUT                     : out   std_logic;
    
    -- FMC HPC Connector
    CONN_LVDS_P                    : in    std_logic_vector(23-1 downto 0);
    CONN_LVDS_N                    : in    std_logic_vector(23-1 downto 0);
    CONN_J1_38_P                   : out   std_logic;
    CONN_J1_40_N                   : out   std_logic;

    -- FMC GTH
    MGT_BA_RX0_P                   : in    std_logic;
    MGT_BA_RX0_N                   : in    std_logic;
    MGT_BA_TX0_P                   : out   std_logic;
    MGT_BA_TX0_N                   : out   std_logic;

    MGT_BA_RX1_P                   : in    std_logic;
    MGT_BA_RX1_N                   : in    std_logic;
    MGT_BA_TX1_P                   : out   std_logic;
    MGT_BA_TX1_N                   : out   std_logic;

    MGT_BA_RX2_P                   : in    std_logic;
    MGT_BA_RX2_N                   : in    std_logic;
    MGT_BA_TX2_P                   : out   std_logic;
    MGT_BA_TX2_N                   : out   std_logic;

    MGT_BA_RX3_P                   : in    std_logic;
    MGT_BA_RX3_N                   : in    std_logic;
    MGT_BA_TX3_P                   : out   std_logic;
    MGT_BA_TX3_N                   : out   std_logic;

    -- I2C FPGA
    I2C_SCL_FPGA                   : inout std_logic;
    I2C_SDA_FPGA                   : inout std_logic;
    
    BTN1_N                         : in    std_logic; -- ST1 BUTTONS
    PL_LED2_N                      : out   std_logic; -- XU8 LED
    LED2                           : out   std_logic; -- ST1 LED
    LED3                           : out   std_logic  -- ST1 LED
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

signal user_rx_clk : std_logic_vector(4-1 downto 0);
signal user_tx_clk : std_logic_vector(4-1 downto 0);

signal qpll0reset  : std_logic_vector(4-1 downto 0);
signal qpll0lock   : std_logic;
signal qpll0clk    : std_logic;
signal qpll0refclk : std_logic;
signal qpll1lock   : std_logic;
signal qpll1clk    : std_logic;
signal qpll1refclk : std_logic;

signal ref2_clk  : std_logic;
signal ps_50_clk : std_logic;
signal ps_50_rst : std_logic;

signal btn1 : std_logic;

signal led2_n : std_logic_vector(4-1 downto 0);
signal led3_n : std_logic_vector(4-1 downto 0);

--------------------------------------------------------------------------------
-- Implementation
--------------------------------------------------------------------------------
begin
--------------------------------------------------------------------------------
-- Signal connections
--------------------------------------------------------------------------------
DP_AUX_OE <= not dp_aux_data_oe_n;

btn1 <= not BTN1_N;

CONN_J1_38_P <= '0'; --TODO
CONN_J1_40_N <= '0'; --TODO

LED2 <= not led2_n(0); -- LED asserted when low.
LED3 <= not led3_n(0); -- LED asserted when low.

--------------------------------------------------------------------------------
-- Processes
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
-- Modules
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
IBUFDS_GTE4_MGTREFCLK0_X0Y1_INST: IBUFDS_GTE4
  generic map (
    REFCLK_EN_TX_PATH  =>  '0',
    REFCLK_HROW_CK_SEL =>  "00",
    REFCLK_ICNTL_RX    =>  "00"
  ) 
  port map
  (
    I     => CLK_REF2_P,
    IB    => CLK_REF2_N,
    CEB   => '0',
    O     => ref2_clk,
    ODIV2 => open
  );
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
gth_commons_i: entity work.gth_commons
  port map
  (
    refclk0    => ref2_clk,
    qpll0reset => qpll0reset(0),
     
    qpll0lock      => qpll0lock,
    qpll0outclk    => qpll0clk,
    qpll0outrefclk => qpll0refclk,
    qpll1lock      => qpll1lock,
    qpll1outclk    => qpll1clk,
    qpll1outrefclk => qpll1refclk
  );
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
aurora_64b66b_0_i: entity work.aurora_64b66b_0
  port map
  (
    hb_gtwiz_reset_clk_freerun_in => ps_50_clk,
    hb_gtwiz_reset_all_in         => ps_50_rst,
    
    ch0_gthrxn_in  => MGT_BA_RX0_N,
    ch0_gthrxp_in  => MGT_BA_RX0_P,
    ch0_gthtxn_out => MGT_BA_TX0_N,
    ch0_gthtxp_out => MGT_BA_TX0_P,

    link_down_latched_reset_in => btn1,
    
    link_status_out     => led2_n(0),
    link_down_latched_o => led3_n(0),
    
    user_rx_clk_o => user_rx_clk(0),
    user_tx_clk_o => user_tx_clk(0),
    
    qpll0reset_o  => qpll0reset(0),
    qpll0lock_i   => qpll0lock,
    qpll0clk_i    => qpll0clk,
    qpll0refclk_i => qpll0refclk,
    qpll1lock_i   => qpll1lock,
    qpll1clk_i    => qpll1clk,
    qpll1refclk_i => qpll1refclk
  );
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
aurora_64b66b_1_i: entity work.aurora_64b66b_1
  port map
  (
    hb_gtwiz_reset_clk_freerun_in => ps_50_clk,
    hb_gtwiz_reset_all_in         => ps_50_rst,
    
    ch0_gthrxn_in  => MGT_BA_RX1_N,
    ch0_gthrxp_in  => MGT_BA_RX1_P,
    ch0_gthtxn_out => MGT_BA_TX1_N,
    ch0_gthtxp_out => MGT_BA_TX1_P,

    link_down_latched_reset_in => btn1,
    
    link_status_out     => led2_n(1),
    link_down_latched_o => led3_n(1),
    
    user_rx_clk_o => user_rx_clk(1),
    user_tx_clk_o => user_tx_clk(1),
    
    qpll0reset_o  => qpll0reset(1),
    qpll0lock_i   => qpll0lock,
    qpll0clk_i    => qpll0clk,
    qpll0refclk_i => qpll0refclk,
    qpll1lock_i   => qpll1lock,
    qpll1clk_i    => qpll1clk,
    qpll1refclk_i => qpll1refclk
  );
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
aurora_64b66b_2_i: entity work.aurora_64b66b_2
  port map
  (
    hb_gtwiz_reset_clk_freerun_in => ps_50_clk,
    hb_gtwiz_reset_all_in         => ps_50_rst,
    
    ch0_gthrxn_in  => MGT_BA_RX2_N,
    ch0_gthrxp_in  => MGT_BA_RX2_P,
    ch0_gthtxn_out => MGT_BA_TX2_N,
    ch0_gthtxp_out => MGT_BA_TX2_P,

    link_down_latched_reset_in => btn1,
    
    link_status_out     => led2_n(2),
    link_down_latched_o => led3_n(2),
    
    user_rx_clk_o => user_rx_clk(2),
    user_tx_clk_o => user_tx_clk(2),
    
    qpll0reset_o  => qpll0reset(2),
    qpll0lock_i   => qpll0lock,
    qpll0clk_i    => qpll0clk,
    qpll0refclk_i => qpll0refclk,
    qpll1lock_i   => qpll1lock,
    qpll1clk_i    => qpll1clk,
    qpll1refclk_i => qpll1refclk
  );
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
aurora_64b66b_3_i: entity work.aurora_64b66b_3
  port map
  (
    hb_gtwiz_reset_clk_freerun_in => ps_50_clk,
    hb_gtwiz_reset_all_in         => ps_50_rst,
    
    ch0_gthrxn_in  => MGT_BA_RX3_N,
    ch0_gthrxp_in  => MGT_BA_RX3_P,
    ch0_gthtxn_out => MGT_BA_TX3_N,
    ch0_gthtxp_out => MGT_BA_TX3_P,

    link_down_latched_reset_in => btn1,
    
    link_status_out     => led2_n(3),
    link_down_latched_o => led3_n(3),
    
    user_rx_clk_o => user_rx_clk(3),
    user_tx_clk_o => user_tx_clk(3),
    
    qpll0reset_o  => qpll0reset(3),
    qpll0lock_i   => qpll0lock,
    qpll0clk_i    => qpll0clk,
    qpll0refclk_i => qpll0refclk,
    qpll1lock_i   => qpll1lock,
    qpll1clk_i    => qpll1clk,
    qpll1refclk_i => qpll1refclk
  );
--------------------------------------------------------------------------------

--------------------------------------------------------------------------------
system_wrapper_i: entity work.system_wrapper
  port map
  (
    PS_50_CLK => ps_50_clk,
    PS_50_RST => ps_50_rst,
    
    USER_CLK => user_rx_clk(0),
    
    I2C_FPGA_scl_io => I2C_SCL_FPGA,
    I2C_FPGA_sda_io => I2C_SDA_FPGA,

    PL_LED2_N => PL_LED2_N,

    DP_AUX_OUT => DP_AUX_OUT,
    DP_AUX_OE  => dp_aux_data_oe_n,
    DP_AUX_IN  => DP_AUX_IN,
    DP_HPD     => DP_HPD
  );
--------------------------------------------------------------------------------
  
end rtl;
