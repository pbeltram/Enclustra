
set path_src_ip ./hdl_aurora_64b66b

add_files -norecurse $path_src_ip/gt_aurora_64b66b_example_bit_sync.v
add_files -norecurse $path_src_ip/gt_aurora_64b66b_example_checking_64b66b.v
add_files -norecurse $path_src_ip/gt_aurora_64b66b_example_gtwiz_userclk_rx.v
add_files -norecurse $path_src_ip/gt_aurora_64b66b_example_gtwiz_userclk_tx.v
add_files -norecurse $path_src_ip/gt_aurora_64b66b_example_init.v
add_files -norecurse $path_src_ip/gt_aurora_64b66b_example_reset_sync.v
add_files -norecurse $path_src_ip/gt_aurora_64b66b_example_stimulus_64b66b.v
add_files -norecurse $path_src_ip/gt_aurora_64b66b_prbs_any.v
add_files -norecurse $path_src_ip/gt_aurora_64b66b_gthe4_common_wrapper.v
add_files -norecurse $path_src_ip/gtwizard_ultrascale_v1_7_gthe4_common.v
add_files -norecurse $path_src_ip/gt_aurora_64b66b_example_gtwiz_reset.v
add_files -norecurse $path_src_ip/gt_aurora_64b66b_example_reset_inv_sync.v
add_files -norecurse $path_src_ip/gth_commons.v
add_files -norecurse $path_src_ip/gt_aurora_64b66b_example_wrapper_functions.vh

import_ip ${path_src_ip}_0/gt_aurora_64b66b_0_vio_0.xci -name gt_aurora_64b66b_0_vio_0
import_ip ${path_src_ip}_0/gt_aurora_64b66b_0_in_system_ibert_0.xci -name gt_aurora_64b66b_0_in_system_ibert_0
import_ip ${path_src_ip}_0/gt_aurora_64b66b_0.xci -name gt_aurora_64b66b_0
add_files -norecurse ${path_src_ip}_0/gt_aurora_64b66b_0_example_wrapper.v
add_files -norecurse ${path_src_ip}_0/aurora_64b66b_0.v

import_ip ${path_src_ip}_1/gt_aurora_64b66b_1_vio_0.xci -name gt_aurora_64b66b_1_vio_0
import_ip ${path_src_ip}_1/gt_aurora_64b66b_1_in_system_ibert_0.xci -name gt_aurora_64b66b_1_in_system_ibert_0
import_ip ${path_src_ip}_1/gt_aurora_64b66b_1.xci -name gt_aurora_64b66b_1
add_files -norecurse ${path_src_ip}_1/gt_aurora_64b66b_1_example_wrapper.v
add_files -norecurse ${path_src_ip}_1/aurora_64b66b_1.v

import_ip ${path_src_ip}_2/gt_aurora_64b66b_2_vio_0.xci -name gt_aurora_64b66b_2_vio_0
import_ip ${path_src_ip}_2/gt_aurora_64b66b_2_in_system_ibert_0.xci -name gt_aurora_64b66b_2_in_system_ibert_0
import_ip ${path_src_ip}_2/gt_aurora_64b66b_2.xci -name gt_aurora_64b66b_2
add_files -norecurse ${path_src_ip}_2/gt_aurora_64b66b_2_example_wrapper.v
add_files -norecurse ${path_src_ip}_2/aurora_64b66b_2.v

import_ip ${path_src_ip}_3/gt_aurora_64b66b_3_vio_0.xci -name gt_aurora_64b66b_3_vio_0
import_ip ${path_src_ip}_3/gt_aurora_64b66b_3_in_system_ibert_0.xci -name gt_aurora_64b66b_3_in_system_ibert_0
import_ip ${path_src_ip}_3/gt_aurora_64b66b_3.xci -name gt_aurora_64b66b_3
add_files -norecurse ${path_src_ip}_3/gt_aurora_64b66b_3_example_wrapper.v
add_files -norecurse ${path_src_ip}_3/aurora_64b66b_3.v

