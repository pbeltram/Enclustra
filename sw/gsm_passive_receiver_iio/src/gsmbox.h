/*
 * gsmbox.h
 *
 *  Created on: Oct 7, 2023
 *      Author: primozb
 */

#ifndef INCLUDED_gsmbox_gsmbox_h
#define INCLUDED_gsmbox_gsmbox_h

#include "gr_complex.h"

//----------------------------------------------------------------------------//
uint32_t create_lpfir_filter(double a_samp_rate, uint32_t a_len_out, float *a_fir_coeffs_out);
void get_train_seq_iq(uint32_t a_seq_no, gr_complex *a_train_seq_out);
uint32_t filter_data(uint32_t a_fir_len, float *a_fir_coeffs,  double a_phase_inc,
    uint32_t a_data_len, gr_complex *a_data, gr_complex *a_data_out);
uint32_t rotate_data(double a_phase_inc, uint32_t a_data_len, gr_complex *a_data, gr_complex *a_data_out);
uint32_t get_burst_start(uint32_t a_osr, double a_mwa_threshold, uint32_t a_data_len, gr_complex *a_data);
uint32_t get_burst_offset(uint32_t a_osr, gr_complex *a_train_seq, gr_complex *a_data);
void get_imp_resp(uint32_t a_osr, gr_complex *a_train_seq, gr_complex *a_data, gr_complex *a_imp_resp_out);
float get_imp_resp_max(uint32_t a_osr, gr_complex *a_imp_resp);
void mafi_burst(uint32_t a_osr, gr_complex *a_imp_resp, gr_complex *a_data_in, gr_complex *a_filter_out);
void demod_burst(uint32_t a_osr, gr_complex *a_imp_resp, gr_complex *a_filtered, uint8_t *a_binary_out);
double get_phase_inc(uint32_t a_osr, float a_freq_offs);
float get_freq_offset(uint32_t a_osr, double a_phase_inc);
uint32_t decode_bursts(uint8_t *a_bursts_u, uint32_t a_bursts_u_len, uint32_t a_len_out, uint8_t *a_result);
//----------------------------------------------------------------------------//

#endif //!INCLUDED_gsmbox_gsmbox_h
