/*
 * gsmbox.cpp
 *
 *  Created on: Oct 7, 2023
 *      Author: primozb
 */

#include <algorithm>
#include <cassert>
#include <iostream>
#include <cstring>
#include <list>
#include <iomanip>

#include "gsm_constants.h"
#include "gsmbox.h"
#include "gr_firdes.h"
#include "viterbi_detector.h"
#include "rotator.h"
#include "decoder.h"

//----------------------------------------------------------------------------//
uint32_t create_lpfir_filter(double a_samp_rate, uint32_t a_len_out, float *a_fir_coeffs_out)
{
  std::vector<float> fir_taps = gr_firdes::low_pass(1.0, a_samp_rate, 125000.0, 5000.0, gr_firdes::WIN_HAMMING, 6.76);
  std::reverse(fir_taps.begin(), fir_taps.end());

  size_t fir_len = fir_taps.size();
  if (a_len_out >= fir_len) {
    std::copy(fir_taps.begin(), fir_taps.end(), a_fir_coeffs_out);
  }

  return fir_len;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
static void gmsk_mapper(const unsigned char * input, int nitems, gr_complex *gmsk_output, gr_complex start_point)
{
  gr_complex j = gr_complex(0.0, 1.0);
  gmsk_output[0] = start_point;

  int previous_symbol = 2 * input[0] - 1;
  int current_symbol;
  int encoded_symbol;

  for (int i = 1; i < nitems; i++) {
    // Change bits representation to NRZ
    current_symbol = 2 * input[i] - 1;

    // Differentially encode
    encoded_symbol = current_symbol * previous_symbol;

    // And do GMSK mapping
    gmsk_output[i] = j * gr_complex(encoded_symbol, 0.0) * gmsk_output[i-1];

    previous_symbol = current_symbol;
  }
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
void get_train_seq_iq(uint32_t a_seq_no, gr_complex *a_train_seq_out)
{
  if (a_seq_no > TRAIN_SEQ_NUM) {
    return;
  }

  // If first bit of the sequence is 0 => first symbol is 1, else -1
  gr_complex startpoint = train_seq[a_seq_no][0] == 0 ? gr_complex(1.0, 0.0) : gr_complex(-1.0, 0.0);
  gmsk_mapper(train_seq[a_seq_no], N_TRAIN_BITS, a_train_seq_out, startpoint);

  return;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
static void autocorrelation(const gr_complex *input, gr_complex *out, int nitems)
{
  for (int k = nitems - 1; k >= 0; k--) {
    out[k] = gr_complex(0, 0);
    for (int i = k; i < nitems; i++)
      out[k] += input[i] * conj(input[i - k]);
  }
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
void demod_burst(uint32_t a_osr, gr_complex *a_imp_resp,
    gr_complex *a_filtered, uint8_t *a_binary_out)
{
  gr_complex rhh_temp[CHAN_IMP_RESP_LENGTH * a_osr];
  unsigned int stop_states[2] = {4, 12};
  gr_complex rhh[CHAN_IMP_RESP_LENGTH];
  float output[BURST_SIZE];
  int start_state = 3;

  autocorrelation(a_imp_resp, rhh_temp, CHAN_IMP_RESP_LENGTH * a_osr);
  for (int ii = 0; ii < CHAN_IMP_RESP_LENGTH; ii++)
    rhh[ii] = conj(rhh_temp[ii * a_osr]);

  viterbi_detector(a_filtered, BURST_SIZE, rhh, start_state, stop_states, 2, output);

  for (int i = 0; i < BURST_SIZE; i++)
    a_binary_out[i] = uint8_t(output[i] > 0);

  return;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
void mafi_burst(uint32_t a_osr, gr_complex *a_imp_resp, gr_complex *a_data_in,
    gr_complex *a_filter_out)
{
  for (int n = 0; n < BURST_SIZE; n++) {
    int a = n * a_osr;
    a_filter_out[n] = 0;

    for (int ii = 0; ii < (CHAN_IMP_RESP_LENGTH*a_osr); ii++) {
      if ((a + ii) >= (BURST_SIZE*a_osr))
        break;

      a_filter_out[n] += a_data_in[a + ii] * a_imp_resp[ii];
    }
  }
  return;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
static gr_complex correlate_sequence(uint32_t a_osr, const gr_complex *sequence,
    int length, const gr_complex *input)
{
  gr_complex result(0.0, 0.0);

  for (int ii = 0; ii < length; ii++)
    result += sequence[ii] * conj(input[ii * a_osr]);

  return result / gr_complex(length, 0);
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
uint32_t get_burst_offset(uint32_t a_osr, gr_complex *a_train_seq, gr_complex *a_data)
{
  std::vector<float> window_energy_buffer;
  std::vector<float> power_buffer;

//  int search_center = (int) ((floor(PROCESSED_CHUNK*a_osr)+1) / 2) - ((N_TRAIN_BITS / 2) - TRAIN_BEGINNING)*a_osr;
  int search_center = ((BURST_SIZE/2+GUARD_BITS) - ((N_TRAIN_BITS/2) - TRAIN_BEGINNING))*a_osr;
  int search_start_pos = search_center + 1 - 5 * a_osr;
  int search_stop_pos = search_center + CHAN_IMP_RESP_LENGTH * a_osr + 5 * a_osr;

  for (int ii = search_start_pos; ii < search_stop_pos; ii++) {
    gr_complex correlation = correlate_sequence(a_osr, &a_train_seq[TRAIN_BEGINNING], N_TRAIN_BITS - 10, &a_data[ii]);

    #if !defined(NDEBUG)
    std::cout
        << " idx=" << std::dec << (ii-search_start_pos)
        << " pos=" << std::dec << ii
        << " input=(" << std::fixed << std::setfill(' ') << std::setw(7) << std::setprecision(4) << a_data[ii].real()
        << "," << std::fixed << std::setfill(' ') << std::setw(7) << std::setprecision(4) << a_data[ii].imag() << ")"
        << " corr=(" << std::fixed << std::setfill(' ') << std::setw(7) << std::setprecision(4) << correlation.real()
        << "," << std::fixed << std::setfill(' ') << std::setw(7) << std::setprecision(4) << correlation.imag() << ")"
        << std::endl;
    #endif

    power_buffer.push_back(std::pow(abs(correlation), 2));
  }

  std::vector<float>::iterator iter = power_buffer.begin();
  while (iter != power_buffer.end()) {
    std::vector<float>::iterator iter_ii = iter;
    bool loop_end = false;
    float energy = 0;

    int len = CHAN_IMP_RESP_LENGTH * a_osr;
    for (int ii = 0; ii < len; ii++, iter_ii++) {
      if (iter_ii == power_buffer.end()) {
        loop_end = true;
        break;
      }

      energy += (*iter_ii);
    }

    if (loop_end)
      break;

    window_energy_buffer.push_back(energy);
    iter++;
  }

  int strongest_window_nr = max_element(window_energy_buffer.begin(),
      window_energy_buffer.end() - CHAN_IMP_RESP_LENGTH * a_osr) - window_energy_buffer.begin();

  if (strongest_window_nr < 0)
    strongest_window_nr = 0;

  return search_start_pos + strongest_window_nr - TRAIN_POS * a_osr;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
float get_imp_resp_max(uint32_t a_osr, gr_complex *a_imp_resp)
{
  float max_correlation = 0;
  for (int ii = 0; ii < CHAN_IMP_RESP_LENGTH * a_osr; ii++) {
    float correlation = abs(a_imp_resp[ii]);
    if (correlation > max_correlation)
      max_correlation = correlation;
  }
  return max_correlation;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
void get_imp_resp(uint32_t a_osr, gr_complex *a_train_seq, gr_complex *a_data, gr_complex *a_imp_resp_out)
{
  int start_pos = (BURST_SIZE/2 - ((N_TRAIN_BITS/2) - TRAIN_BEGINNING))*a_osr;

  for (int ii = 0; ii < (CHAN_IMP_RESP_LENGTH * a_osr); ii++) {
    a_imp_resp_out[ii] = correlate_sequence(a_osr, &a_train_seq[TRAIN_BEGINNING], N_TRAIN_BITS-10, &a_data[ii+start_pos]);

    #if !defined(NDEBUG)
    std::cout
        << " idx=" << std::dec << (ii)
        << " pos=" << std::dec << (ii+start_pos)
        << " input=(" << std::fixed << std::setfill(' ') << std::setw(7) << std::setprecision(4) << a_data[ii+start_pos].real()
        << "," << std::fixed << std::setfill(' ') << std::setw(7) << std::setprecision(4) << a_data[ii+start_pos].imag() << ")"
        << " corr=(" << std::fixed << std::setfill(' ') << std::setw(7) << std::setprecision(4) << a_imp_resp_out[ii].real()
        << "," << std::fixed << std::setfill(' ') << std::setw(7) << std::setprecision(4) << a_imp_resp_out[ii].imag() << ")"
        << std::endl;
    #endif

  }

  return;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
uint32_t get_burst_start(uint32_t a_osr, double a_mwa_threshold,
    uint32_t a_data_len, gr_complex *a_data)
{
  size_t mva_len = (BURST_SIZE*a_osr);
  std::list<double> mva_buffer(mva_len, double(0));
  double mva_sum = 0;

  uint32_t counter = 0;

  uint32_t burst_start_pos = 0;

  double mva_max;
  gr_complex sample;

  enum states { search, find_max } burst_search_state;

  burst_search_state = search;
  bool end = false;

  while ((counter < a_data_len) && (end == false))
  {
    switch (burst_search_state) {
      case search:
      {
        sample = a_data[counter];
        counter++;

        double sig_abs = abs(sample);
        mva_sum = mva_sum + (sig_abs - mva_buffer.front());
        mva_buffer.pop_front();
        mva_buffer.push_back(sig_abs);

        double mva = mva_sum/mva_buffer.size();
        if (mva > a_mwa_threshold) {
          mva_max = mva;
          burst_search_state = find_max;
        }

        break;
      }

      case find_max:
      {
        sample = a_data[counter];
        counter++;

        double sig_abs = abs(sample);
        mva_sum = mva_sum + (sig_abs - mva_buffer.front());
        mva_buffer.pop_front();
        mva_buffer.push_back(sig_abs);

        double mva = mva_sum/mva_buffer.size();
        if (mva > mva_max) {
          mva_max = mva;
        }
        else {
          // Max value is at two samples back.
          burst_start_pos = (counter-2) - ((BURST_SIZE+GUARD_BITS)*a_osr);
          end = true;
        }

        break;
      }
    }
  }

  return burst_start_pos;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
uint32_t filter_data(uint32_t a_fir_len, float *a_fir_coeffs, double a_phase_inc,
    uint32_t a_data_len, gr_complex *a_data, gr_complex *a_data_out)
{
  uint32_t fir_flush = a_fir_len;
  uint32_t fir_len = a_fir_len;
  std::list<gr_complex> fir_buffer(a_fir_len, gr_complex(0, 0));

  rotator rotate;
  rotate.set_phase_incr(exp(gr_complex(0, a_phase_inc)));

  uint32_t counter = 0;
  uint32_t counter_out = 0;

  while (counter < a_data_len)
  {
    gr_complex sample = a_data[counter++];
    sample = rotate.rotate(sample);

    // Pass through FIR filter.
    size_t fir_idx(0);
    gr_complex fir_sum(0, 0);

    fir_buffer.pop_back();
    fir_buffer.push_front(sample);

    for (auto citer = fir_buffer.cbegin(); citer != fir_buffer.cend(); citer++) {
      fir_sum += (*citer * a_fir_coeffs[fir_idx++]);
    }
    assert((fir_idx == fir_len) && ":size of FIR.");

    a_data_out[counter_out++] = fir_sum;
  }

  //Flush FIR.
  while (fir_flush-- != 0)
  {
    gr_complex sample = gr_complex(0, 0);
    sample = rotate.rotate(sample);

    // Pass through FIR filter.
    size_t fir_idx(0);
    gr_complex fir_sum(0, 0);

    fir_buffer.pop_back();
    fir_buffer.push_front(sample);

    for (auto citer = fir_buffer.cbegin(); citer != fir_buffer.cend(); citer++) {
      fir_sum += (*citer * a_fir_coeffs[fir_idx++]);
    }
    assert((fir_idx == fir_len) && ":size of FIR.");

    a_data_out[counter_out++] = fir_sum;
  }

  return counter_out;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
double get_phase_inc(uint32_t a_osr, float a_freq_offs)
{
  double phase_inc = -2*M_PI*a_freq_offs/(a_osr * GSM_SYMBOL_RATE);

  return phase_inc;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
float get_freq_offset(uint32_t a_osr, double a_phase_inc)
{
  float freq_offset = a_phase_inc * (a_osr * GSM_SYMBOL_RATE)/(-2*M_PI);

  return freq_offset;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
uint32_t rotate_data(double a_phase_inc, uint32_t a_data_len, gr_complex *a_data,
    gr_complex *a_data_out)
{
  rotator rotate;
  rotate.set_phase_incr(exp(gr_complex(0, a_phase_inc)));

  uint32_t counter = 0;

  while (counter++ < a_data_len)
  {
    a_data_out[counter] = rotate.rotate(a_data[counter]);
  }

  return counter;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
uint32_t decode_bursts(uint8_t *a_bursts_u, uint32_t a_bursts_u_len,
    uint32_t a_len_out, uint8_t *a_result)
{
  int errors = 0;
  int nof_bits = 0;

  size_t res_len = decode(a_bursts_u, a_bursts_u_len, &errors, &nof_bits, a_result);

  #if !defined(NDEBUG)
  if (res_len) {
    std::cout
        << "Decode ResLen =" << std::dec << (res_len)
        << " errors=" << std::dec << (errors)
        << " bits=" << std::dec << (nof_bits)
        << std::endl;
  }
  #endif

  return res_len;
}
//----------------------------------------------------------------------------//
