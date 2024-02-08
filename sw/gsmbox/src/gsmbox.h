/*
 * gsmbox.h
 *
 *  Created on: Oct 7, 2023
 *      Author: primozb
 */

#ifndef INCLUDED_gsmbox_gsmbox_h
#define INCLUDED_gsmbox_gsmbox_h

#include "data_types.h"
#include "gr_complex.h"
#include "gsm_constants.h"

namespace gsmbox {

//----------------------------------------------------------------------------//
GSMBOX_API const uint32_t c_chan_imp_resp_length = CHAN_IMP_RESP_LENGTH;
GSMBOX_API const float    c_processed_chunk = PROCESSED_CHUNK;
GSMBOX_API const uint32_t c_burst_size = BURST_SIZE;
GSMBOX_API const uint32_t c_train_seq_length = N_TRAIN_BITS;
GSMBOX_API const float    c_gsm_symbol_rate = GSM_SYMBOL_RATE;
GSMBOX_API const uint32_t c_training_pos = (TAIL_BITS+DATA_BITS+STEALING_BIT);

GSMBOX_API uint32_t create_lpfir_filter(double a_samp_rate, uint32_t a_len_out, float *a_fir_coeffs_out);
GSMBOX_API uint32_t get_train_seq_iq(uint32_t a_seq_no, uint32_t a_len_out, gr_complex *a_train_seq_out);
GSMBOX_API uint32_t filter_data(uint32_t a_fir_len, float *a_fir_coeffs,  double a_phase_inc,
    uint32_t a_data_len, gr_complex *a_data, gr_complex *a_data_out);
GSMBOX_API uint32_t rotate_data(double a_phase_inc, uint32_t a_data_len, gr_complex *a_data, gr_complex *a_data_out);
GSMBOX_API uint32_t get_burst_start(uint32_t a_osr, double a_mwa_threshold, uint32_t a_data_len, gr_complex *a_data);
GSMBOX_API uint32_t get_burst_offset(uint32_t a_osr, gr_complex *a_train_seq, gr_complex *a_data);
GSMBOX_API uint32_t get_imp_resp(uint32_t a_osr, gr_complex *a_train_seq, gr_complex *a_data,
    uint32_t a_len_out, gr_complex *a_imp_resp_out);
GSMBOX_API uint32_t mafi_burst(uint32_t a_osr, gr_complex *a_imp_resp, gr_complex *a_data_in,
    uint32_t a_len_out, gr_complex *a_filter_out);
GSMBOX_API uint32_t demod_burst(uint32_t a_osr, gr_complex *a_imp_resp, gr_complex *a_filtered,
    uint32_t a_len_out, uint8_t *a_binary_out);
GSMBOX_API double get_phase_inc(uint32_t a_osr, float a_freq_offs);
GSMBOX_API float get_freq_offset(uint32_t a_osr, double a_phase_inc);
GSMBOX_API uint32_t decode_bursts(uint8_t *a_bursts, uint32_t a_len_out, uint8_t *a_result);
//----------------------------------------------------------------------------//

} // namespace gsmbox

#endif //!INCLUDED_gsmbox_gsmbox_h
