`timescale 1ns / 1ps

//------------------------------------------------------------------------------
module gth_commons
(
  input  refclk0,
  input  qpll0reset,
 
  output qpll0lock,
  output qpll0outclk,
  output qpll0outrefclk,
  output qpll1lock,
  output qpll1outclk,
  output qpll1outrefclk
);

  // ===================================================================================================================
  // TRANSCEIVER COMMON BLOCK
  // ===================================================================================================================

  wire [0:0] gtrefclk00_int;
  wire [0:0] qpll0clk_int;
  wire [0:0] qpll0refclk_int;
  wire [0:0] qpll0reset_int;
  wire [0:0] qpll0lock_int;

  // When QPLL0 is used, the following assignments support the use of the transceiver COMMON block in the example design
  assign gtrefclk00_int = refclk0;
  assign qpll0reset_int = qpll0reset;
  assign qpll0lock      = qpll0lock_int[0];
  assign qpll0outclk    = qpll0clk_int[0];
  assign qpll0outrefclk = qpll0refclk_int[0];
  
//  genvar gi_ch_to_cm0;
//  generate for (gi_ch_to_cm0 = 0; gi_ch_to_cm0 < 1; gi_ch_to_cm0 = gi_ch_to_cm0 + 1) begin : gen_ch_to_cm0
//    assign qpll0clk_int    [gi_ch_to_cm0] = qpll0outclk_out    [f_idx_cm(gi_ch_to_cm0)];
//    assign qpll0refclk_int [gi_ch_to_cm0] = qpll0outrefclk_out [f_idx_cm(gi_ch_to_cm0)];
//  end
//  endgenerate

  wire [0:0] gtrefclk01_int;
  wire [0:0] qpll1clk_int;
  wire [0:0] qpll1refclk_int;
  wire [0:0] qpll1reset_int;
  wire [0:0] qpll1lock_int;

  // When QPLL1 is not used, the following assignments tie off QPLL1-related inputs as appropriate
  assign gtrefclk01_int  = {1{1'b0}};
  assign qpll1reset_int  = {1{1'b1}};
//  assign qpll1clk_int    = {1{1'b0}};
//  assign qpll1refclk_int = {1{1'b0}};

  assign qpll1lock      = qpll1lock_int[0];
  assign qpll1outclk    = qpll1clk_int[0];
  assign qpll1outrefclk = qpll1refclk_int[0];

//  localparam [47:0] P_COMMON_ENABLE = f_pop_cm_en(0);

  // The following HDL generate loop iterates across each possible transceiver quad, instantiating only the enabled
  // transceiver COMMON blocks, each within a configuration-specific parameterization wrapper
//  genvar cm;
//  generate for (cm = 0; cm < 48; cm = cm + 1) begin : gen_common_container
//    if (P_COMMON_ENABLE[cm] == 1'b1) begin : gen_enabled_common

      gt_aurora_64b66b_gthe4_common_wrapper gthe4_common_wrapper_inst (
        .GTHE4_COMMON_BGBYPASSB         (1'b1),
        .GTHE4_COMMON_BGMONITORENB      (1'b1),
        .GTHE4_COMMON_BGPDB             (1'b1),
        .GTHE4_COMMON_BGRCALOVRD        (5'b11111),
        .GTHE4_COMMON_BGRCALOVRDENB     (1'b1),
        .GTHE4_COMMON_DRPADDR           (16'b0000000000000000),
        .GTHE4_COMMON_DRPCLK            (1'b0),
        .GTHE4_COMMON_DRPDI             (16'b0000000000000000),
        .GTHE4_COMMON_DRPEN             (1'b0),
        .GTHE4_COMMON_DRPWE             (1'b0),
        .GTHE4_COMMON_GTGREFCLK0        (1'b0),
        .GTHE4_COMMON_GTGREFCLK1        (1'b0),
        .GTHE4_COMMON_GTNORTHREFCLK00   (1'b0),
        .GTHE4_COMMON_GTNORTHREFCLK01   (1'b0),
        .GTHE4_COMMON_GTNORTHREFCLK10   (1'b0),
        .GTHE4_COMMON_GTNORTHREFCLK11   (1'b0),
        .GTHE4_COMMON_GTREFCLK00        (gtrefclk00_int/* [f_ub_cm( 1,(4*cm)+3) : f_lb_cm( 1,4*cm)]*/),
        .GTHE4_COMMON_GTREFCLK01        (gtrefclk01_int/* [f_ub_cm( 1,(4*cm)+3) : f_lb_cm( 1,4*cm)]*/),
        .GTHE4_COMMON_GTREFCLK10        (1'b0),
        .GTHE4_COMMON_GTREFCLK11        (1'b0),
        .GTHE4_COMMON_GTSOUTHREFCLK00   (1'b0),
        .GTHE4_COMMON_GTSOUTHREFCLK01   (1'b0),
        .GTHE4_COMMON_GTSOUTHREFCLK10   (1'b0),
        .GTHE4_COMMON_GTSOUTHREFCLK11   (1'b0),
        .GTHE4_COMMON_PCIERATEQPLL0     (3'b000),
        .GTHE4_COMMON_PCIERATEQPLL1     (3'b000),
        .GTHE4_COMMON_PMARSVD0          (8'b00000000),
        .GTHE4_COMMON_PMARSVD1          (8'b00000000),
        .GTHE4_COMMON_QPLL0CLKRSVD0     (1'b0),
        .GTHE4_COMMON_QPLL0CLKRSVD1     (1'b0),
        .GTHE4_COMMON_QPLL0FBDIV        (8'b00000000),
        .GTHE4_COMMON_QPLL0LOCKDETCLK   (1'b0),
        .GTHE4_COMMON_QPLL0LOCKEN       (1'b1),
        .GTHE4_COMMON_QPLL0PD           (1'b0),
        .GTHE4_COMMON_QPLL0REFCLKSEL    (3'b001),
        .GTHE4_COMMON_QPLL0RESET        (qpll0reset_int/* [f_ub_cm( 1,(4*cm)+3) : f_lb_cm( 1,4*cm)]*/),
        .GTHE4_COMMON_QPLL1CLKRSVD0     (1'b0),
        .GTHE4_COMMON_QPLL1CLKRSVD1     (1'b0),
        .GTHE4_COMMON_QPLL1FBDIV        (8'b00000000),
        .GTHE4_COMMON_QPLL1LOCKDETCLK   (1'b0),
        .GTHE4_COMMON_QPLL1LOCKEN       (1'b0),
        .GTHE4_COMMON_QPLL1PD           (1'b1),
        .GTHE4_COMMON_QPLL1REFCLKSEL    (3'b001),
        .GTHE4_COMMON_QPLL1RESET        (qpll1reset_int/* [f_ub_cm( 1,(4*cm)+3) : f_lb_cm( 1,4*cm)]*/),
        .GTHE4_COMMON_QPLLRSVD1         (8'b00000000),
        .GTHE4_COMMON_QPLLRSVD2         (5'b00000),
        .GTHE4_COMMON_QPLLRSVD3         (5'b00000),
        .GTHE4_COMMON_QPLLRSVD4         (8'b00000000),
        .GTHE4_COMMON_RCALENB           (1'b1),
        .GTHE4_COMMON_SDM0DATA          (25'b0000000000000000000000000),
        .GTHE4_COMMON_SDM0RESET         (1'b0),
        .GTHE4_COMMON_SDM0TOGGLE        (1'b0),
        .GTHE4_COMMON_SDM0WIDTH         (2'b00),
        .GTHE4_COMMON_SDM1DATA          (25'b0000000000000000000000000),
        .GTHE4_COMMON_SDM1RESET         (1'b0),
        .GTHE4_COMMON_SDM1TOGGLE        (1'b0),
        .GTHE4_COMMON_SDM1WIDTH         (2'b00),
        .GTHE4_COMMON_TCONGPI           (10'b0000000000),
        .GTHE4_COMMON_TCONPOWERUP       (1'b0),
        .GTHE4_COMMON_TCONRESET         (2'b00),
        .GTHE4_COMMON_TCONRSVDIN1       (2'b00),
        .GTHE4_COMMON_DRPDO             (),
        .GTHE4_COMMON_DRPRDY            (),
        .GTHE4_COMMON_PMARSVDOUT0       (),
        .GTHE4_COMMON_PMARSVDOUT1       (),
        .GTHE4_COMMON_QPLL0FBCLKLOST    (),
        .GTHE4_COMMON_QPLL0LOCK         (qpll0lock_int/*   [f_ub_cm( 1,(4*cm)+3) : f_lb_cm( 1,4*cm)]*/),
        .GTHE4_COMMON_QPLL0OUTCLK       (qpll0clk_int/*    [f_ub_cm( 1,(4*cm)+3) : f_lb_cm( 1,4*cm)]*/),
        .GTHE4_COMMON_QPLL0OUTREFCLK    (qpll0refclk_int/* [f_ub_cm( 1,(4*cm)+3) : f_lb_cm( 1,4*cm)]*/),
        .GTHE4_COMMON_QPLL0REFCLKLOST   (),
        .GTHE4_COMMON_QPLL1FBCLKLOST    (),
        .GTHE4_COMMON_QPLL1LOCK         (qpll1lock_int/*   [f_ub_cm( 1,(4*cm)+3) : f_lb_cm( 1,4*cm)]*/),
        .GTHE4_COMMON_QPLL1OUTCLK       (qpll1clk_int/*    [f_ub_cm( 1,(4*cm)+3) : f_lb_cm( 1,4*cm)]*/),
        .GTHE4_COMMON_QPLL1OUTREFCLK    (qpll1refclk_int/* [f_ub_cm( 1,(4*cm)+3) : f_lb_cm( 1,4*cm)]*/),
        .GTHE4_COMMON_QPLL1REFCLKLOST   (),
        .GTHE4_COMMON_QPLLDMONITOR0     (),
        .GTHE4_COMMON_QPLLDMONITOR1     (),
        .GTHE4_COMMON_REFCLKOUTMONITOR0 (),
        .GTHE4_COMMON_REFCLKOUTMONITOR1 (),
        .GTHE4_COMMON_RXRECCLK0SEL      (),
        .GTHE4_COMMON_RXRECCLK1SEL      (),
        .GTHE4_COMMON_SDM0FINALOUT      (),
        .GTHE4_COMMON_SDM0TESTDATA      (),
        .GTHE4_COMMON_SDM1FINALOUT      (),
        .GTHE4_COMMON_SDM1TESTDATA      (),
        .GTHE4_COMMON_TCONGPO           (),
        .GTHE4_COMMON_TCONRSVDOUT0      ()
      );

//    end
//  end
//  endgenerate

endmodule
