 # Reference clock contraint for GTX
create_clock  -name gt_sfp_refclk -period 6.400	 [get_ports CLK_REF2_P]

# ----------------------------------------------------------------------------------------------------------------------
# gt_aurora_64b66b

create_clock -name gt0_drp_clk -period 20.000	[get_pins aurora_64b66b_0_i/hb_gtwiz_reset_clk_freerun_in]
set_clock_groups -asynchronous -group [get_clocks gt0_drp_clk -include_generated_clocks]

create_clock -name gt1_drp_clk -period 20.000	[get_pins aurora_64b66b_1_i/hb_gtwiz_reset_clk_freerun_in]
set_clock_groups -asynchronous -group [get_clocks gt1_drp_clk -include_generated_clocks]

create_clock -name gt2_drp_clk -period 20.000	[get_pins aurora_64b66b_2_i/hb_gtwiz_reset_clk_freerun_in]
set_clock_groups -asynchronous -group [get_clocks gt2_drp_clk -include_generated_clocks]

create_clock -name gt3_drp_clk -period 20.000	[get_pins aurora_64b66b_3_i/hb_gtwiz_reset_clk_freerun_in]
set_clock_groups -asynchronous -group [get_clocks gt3_drp_clk -include_generated_clocks]

# False path constraints
set_false_path -to [get_cells -hierarchical -filter {NAME =~ aurora_64b66b_*_i/*bit_synchronizer*inst/i_in_meta_reg}] -quiet
##set_false_path -to [get_cells -hierarchical -filter {NAME =~ /*reset_synchronizer*inst/rst_in_*_reg}] -quiet
set_false_path -to [get_pins -filter {REF_PIN_NAME=~*D} -of_objects [get_cells -hierarchical -filter {NAME =~ aurora_64b66b_*_i/*reset_synchronizer*inst/rst_in_meta*}]] -quiet
set_false_path -to [get_pins -filter {REF_PIN_NAME=~*PRE} -of_objects [get_cells -hierarchical -filter {NAME =~ aurora_64b66b_*_i/*reset_synchronizer*inst/rst_in_meta*}]] -quiet
set_false_path -to [get_pins -filter {REF_PIN_NAME=~*PRE} -of_objects [get_cells -hierarchical -filter {NAME =~ aurora_64b66b_*_i/*reset_synchronizer*inst/rst_in_sync1*}]] -quiet
set_false_path -to [get_pins -filter {REF_PIN_NAME=~*PRE} -of_objects [get_cells -hierarchical -filter {NAME =~ aurora_64b66b_*_i/*reset_synchronizer*inst/rst_in_sync2*}]] -quiet
set_false_path -to [get_pins -filter {REF_PIN_NAME=~*PRE} -of_objects [get_cells -hierarchical -filter {NAME =~ aurora_64b66b_*_i/*reset_synchronizer*inst/rst_in_sync3*}]] -quiet
set_false_path -to [get_pins -filter {REF_PIN_NAME=~*PRE} -of_objects [get_cells -hierarchical -filter {NAME =~ aurora_64b66b_*_i/*reset_synchronizer*inst/rst_in_out*}]] -quiet
set_false_path -to [get_pins -filter {REF_PIN_NAME=~*CLR} -of_objects [get_cells -hierarchical -filter {NAME =~ aurora_64b66b_*_i/*reset_synchronizer*inst/rst_in_meta*}]] -quiet
set_false_path -to [get_pins -filter {REF_PIN_NAME=~*CLR} -of_objects [get_cells -hierarchical -filter {NAME =~ aurora_64b66b_*_i/*reset_synchronizer*inst/rst_in_sync1*}]] -quiet
set_false_path -to [get_pins -filter {REF_PIN_NAME=~*CLR} -of_objects [get_cells -hierarchical -filter {NAME =~ aurora_64b66b_*_i/*reset_synchronizer*inst/rst_in_sync2*}]] -quiet
set_false_path -to [get_pins -filter {REF_PIN_NAME=~*CLR} -of_objects [get_cells -hierarchical -filter {NAME =~ aurora_64b66b_*_i/*reset_synchronizer*inst/rst_in_sync3*}]] -quiet
set_false_path -to [get_pins -filter {REF_PIN_NAME=~*CLR} -of_objects [get_cells -hierarchical -filter {NAME =~ aurora_64b66b_*_i/*reset_synchronizer*inst/rst_in_out*}]] -quiet

set_false_path -to [get_cells -hierarchical -filter {NAME =~ aurora_64b66b_*_i/*gtwiz_userclk_tx_inst/*gtwiz_userclk_tx_active_*_reg}] -quiet
set_false_path -to [get_cells -hierarchical -filter {NAME =~ aurora_64b66b_*_i/*gtwiz_userclk_rx_inst/*gtwiz_userclk_rx_active_*_reg}] -quiet
# ----------------------------------------------------------------------------------------------------------------------

