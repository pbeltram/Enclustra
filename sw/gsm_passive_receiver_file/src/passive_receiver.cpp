// See LICENSE ane README.md files.

#include <cmath>
#include <algorithm>
#include <string.h>
#include <iostream>
#include <numeric>
#include <vector>
#include <iomanip>

#include <assert.h>

#include "gr_complex.h"
#include "passive_receiver.h"
#include "viterbi_detector.h"

//----------------------------------------------------------------------------//
passive_receiver::passive_receiver(int a_osr, const double a_mwa_threshold, callback_funct_t a_get_sample_fptr) :
  m_osr(a_osr),
  m_counter(0),
  m_burst_start_pos(0),
  m_burst_end_pos(0),
  m_burst_buffer(nullptr),
  m_bdata_size((DATA_BITS+STEALING_BIT)*2),
  m_nof_brsts(4),
  m_bursts_u(nullptr),
  m_burst_nr(0),
  m_get_sample_fptr(a_get_sample_fptr),
  m_mwa_threshold(a_mwa_threshold)
{
  // Prepare bits of training sequences
  for (int i = 0; i < TRAIN_SEQ_NUM; i++) {
    // If first bit of the sequence is 0 => first symbol is 1, else -1
    gr_complex startpoint = train_seq[i][0] == 0 ? gr_complex(1.0, 0.0) : gr_complex(-1.0, 0.0);
    gmsk_mapper(train_seq[i], N_TRAIN_BITS, m_norm_training_seq[i], startpoint);
  }

  m_burst_buff_size = floor(PROCESSED_CHUNK*m_osr)+1;
  m_burst_buffer = new gr_complex[m_burst_buff_size];
  m_bursts_u = new uint8_t[m_bdata_size * m_nof_brsts];

  m_mva_len = (BURST_SIZE*m_osr);
  m_mva_buffer = new std::list<double>(m_mva_len, double(0));
  m_mva_sum = 0;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
passive_receiver::~passive_receiver()
{
  delete m_mva_buffer;
  delete [] m_burst_buffer;
  delete [] m_bursts_u;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
bool passive_receiver::work()
{
  bool bRet = burst_search_handler();
  if (bRet == true) {
//    std::cout << "Burst start pos=" << std::dec << m_burst_start_pos << std::endl;
  }

  return bRet;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
inline bool passive_receiver::get_sample(gr_complex &a_sample)
{
  if (m_get_sample_fptr != nullptr) {
    return m_get_sample_fptr(a_sample);
  }
  return false;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
inline void passive_receiver::update_burst_buffer(const gr_complex &a_sample)
{
  ::memmove(reinterpret_cast<void *>(&m_burst_buffer[0]), reinterpret_cast<const void *>(&m_burst_buffer[1]), (m_burst_buff_size-1)*sizeof(gr_complex));
  m_burst_buffer[m_burst_buff_size-1] = a_sample;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
inline double passive_receiver::update_mva(double a_val)
{
  double mva_sum = m_mva_sum + (a_val - m_mva_buffer->front());
  m_mva_buffer->pop_front();
  m_mva_buffer->push_back(a_val);

  return mva_sum;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
bool passive_receiver::find_burst()
{

  double mva_max;
  gr_complex sample;

  bool result = false;
  bool end = false;

  enum states { search, find_max, burst_found, search_fail } burst_search_state;

  // Set initial state //
  burst_search_state = search;

  while (!end)
  {
    switch (burst_search_state) {
      case search:
      {
        if (get_sample(sample) == false) {
          burst_search_state = search_fail;
          break;
        }

        m_counter++;
        update_burst_buffer(sample);

        double sig_abs = abs(sample);
        double signal_pwr = sig_abs*sig_abs;
        signal_pwr = round(10*log10(signal_pwr/50));

        m_mva_sum = update_mva(sig_abs);

        double mva = m_mva_sum/m_mva_buffer->size();
        if (mva > m_mwa_threshold) {
          mva_max = mva;
          burst_search_state = find_max;
        }

        break;
      }

      case find_max:
      {
        if (get_sample(sample) == false) {
          burst_search_state = search_fail;
          break;
        }

        m_counter++;
        update_burst_buffer(sample);

        double sig_abs = abs(sample);
        m_mva_sum = update_mva(sig_abs);

        double mva = m_mva_sum/m_mva_buffer->size();
        if (mva > mva_max) {
          mva_max = mva;
        }
        else {
          // Max value is at two samples back.
          m_burst_start_pos = (m_counter-2) - ((BURST_SIZE+GUARD_BITS)*m_osr);
          burst_search_state = burst_found;
        }

        break;
      }

      case burst_found:
      {
        size_t i = 0;
        //NOTE: Don't get_sample() over i-th index (condition execution)!
        while ((i < size_t((GUARD_BITS*m_osr))) && (get_sample(sample) == true)) {
          i++;
          m_counter++;
          update_burst_buffer(sample);
          double sig_abs = abs(sample);
          m_mva_sum = update_mva(sig_abs);
        }

        if (i == size_t((GUARD_BITS*m_osr))) {
          m_burst_nr++;
          end = true;
          result = true;
          // Clear MVA.
          delete m_mva_buffer;
          m_mva_buffer = new std::list<double>(m_mva_len, double(0));
          m_mva_sum = 0;
        }
        else {
          burst_search_state = search_fail;
        }

        break;
      }

      case search_fail:
        end = true;
        result = false;
        break;
    }
  }

  return result;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
bool passive_receiver::burst_search_handler()
{
  if (!find_burst())
    return false;

  return true;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Especially computations of strongest_window_nr
int passive_receiver::get_norm_chan_imp_resp(const gr_complex *input, gr_complex *chan_imp_resp, float *corr_max, int bcc)
{
  std::vector<gr_complex> correlation_buffer;
  std::vector<float> window_energy_buffer;
  std::vector<float> power_buffer;

  // Captured burst is centered in m_burst_buff.
  int search_center = (int) (m_burst_buff_size / 2) - ((N_TRAIN_BITS / 2) - TRAIN_BEGINNING) * m_osr;
  int search_start_pos = search_center + 1 - 5 * m_osr;
  int search_stop_pos = search_center + CHAN_IMP_RESP_LENGTH * m_osr + 5 * m_osr;

  for (int ii = search_start_pos; ii < search_stop_pos; ii++) {
//    double sig_abs = abs(input[ii]);
    gr_complex correlation = correlate_sequence(&m_norm_training_seq[bcc][TRAIN_BEGINNING], N_TRAIN_BITS - 10, &input[ii]);
    correlation_buffer.push_back(correlation);
    power_buffer.push_back(std::pow(abs(correlation), 2));
  }

  // Compute window energies
  std::vector<float>::iterator iter = power_buffer.begin();
  while (iter != power_buffer.end()) {
    std::vector<float>::iterator iter_ii = iter;
    bool loop_end = false;
    float energy = 0;

    int len = CHAN_IMP_RESP_LENGTH * m_osr;
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

  // Calculate the strongest window number
  int strongest_window_nr = max_element(window_energy_buffer.begin(), window_energy_buffer.end() - CHAN_IMP_RESP_LENGTH * m_osr) - window_energy_buffer.begin();

  if (strongest_window_nr < 0)
    strongest_window_nr = 0;

  float max_correlation = 0;
  for (int ii = 0; ii < CHAN_IMP_RESP_LENGTH * m_osr; ii++) {
    gr_complex correlation = correlation_buffer[strongest_window_nr + ii];
    if (abs(correlation) > max_correlation)
      max_correlation = abs(correlation);

    chan_imp_resp[ii] = correlation;
  }

  *corr_max = max_correlation;

  // Compute first sample position, which corresponds
  // to the first sample of the impulse response
  return search_start_pos + strongest_window_nr - TRAIN_POS * m_osr;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
void passive_receiver::detect_burst(const gr_complex * input, gr_complex * chan_imp_resp, int burst_start, gr_complex * filtered_burst, unsigned char * output_binary)
{
  gr_complex rhh_temp[CHAN_IMP_RESP_LENGTH * m_osr];
  unsigned int stop_states[2] = {4, 12};
//  gr_complex filtered_burst[BURST_SIZE];
  gr_complex rhh[CHAN_IMP_RESP_LENGTH];
  float output[BURST_SIZE];
  int start_state = 3;

  autocorrelation(chan_imp_resp, rhh_temp, CHAN_IMP_RESP_LENGTH * m_osr);
  for (int ii = 0; ii < CHAN_IMP_RESP_LENGTH; ii++)
    rhh[ii] = conj(rhh_temp[ii * m_osr]);

  mafi(&input[burst_start], BURST_SIZE, chan_imp_resp, CHAN_IMP_RESP_LENGTH * m_osr, filtered_burst);

  viterbi_detector(filtered_burst, BURST_SIZE, rhh, start_state, stop_states, 2, output);

  for (int i = 0; i < BURST_SIZE; i++)
    output_binary[i] = output[i] > 0;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
void passive_receiver::gmsk_mapper(const unsigned char * input, int nitems, gr_complex * gmsk_output, gr_complex start_point)
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
gr_complex passive_receiver::correlate_sequence(const gr_complex * sequence, int length, const gr_complex * input)
{
  gr_complex result(0.0, 0.0);

  for (int ii = 0; ii < length; ii++)
    result += sequence[ii] * conj(input[ii * m_osr]);

  return result / gr_complex(length, 0);
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Computes autocorrelation for positive arguments
inline void passive_receiver::autocorrelation(const gr_complex * input, gr_complex * out, int nitems)
{
  for (int k = nitems - 1; k >= 0; k--) {
    out[k] = gr_complex(0, 0);
    for (int i = k; i < nitems; i++)
      out[k] += input[i] * conj(input[i - k]);
  }
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
inline void passive_receiver::mafi(const gr_complex * input, int nitems, gr_complex * filter, int filter_length, gr_complex * output)
{
  for (int n = 0; n < nitems; n++) {
    int a = n * m_osr;
    output[n] = 0;

    for (int ii = 0; ii < filter_length; ii++) {
      if ((a + ii) >= nitems * m_osr)
        break;

      output[n] += input[a + ii] * filter[ii];
    }
  }
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
void passive_receiver::dbg_dump_burst(uint32_t burst_nr, uint32_t a_burst_start, const uint8_t *a_burst_binary, float corr_max)
{
  std::cout << "No=" << std::dec << std::setw(3) << burst_nr
      << " Corr=" << std::fixed << std::setfill(' ') << std::setw(6) << std::setprecision(3) << corr_max
      << " Start=" << std::dec << std::setw(10) << m_burst_start_pos
      << " buff_off=" << a_burst_start
      << " [";

  size_t i = 0;
  for (size_t j = 0; j < 3; j++) {
    std::cout << std::dec << std::setw(1) << static_cast<int>(a_burst_binary[i++]);
  }

  std::cout << "][";
  for (size_t j = 0; j < 57; j++) {
    std::cout << std::dec << std::setw(1) << static_cast<int>(a_burst_binary[i++]);
  }

  std::cout << "][";
  std::cout << std::dec << std::setw(1) << static_cast<int>(a_burst_binary[i++]);

  std::cout << "][";
  for (size_t j = 0; j < N_TRAIN_BITS; j++) {
    std::cout << std::dec << std::setw(1) << static_cast<int>(a_burst_binary[i++]);
  }

  std::cout << "][";
  std::cout << std::dec << std::setw(1) << static_cast<int>(a_burst_binary[i++]);

  std::cout << "][";
  for (size_t j = 0; j < 57; j++) {
    std::cout << std::dec << std::setw(1) << static_cast<int>(a_burst_binary[i++]);
  }
  std::cout << "][";
  for (size_t j = 0; j < 3; j++) {
    std::cout << std::dec << std::setw(1) << static_cast<int>(a_burst_binary[i++]);
  }
  assert((i == BURST_SIZE) && ":dump normal burst buffer.");
  std::cout << "]" << std::endl;

//  std::cout << "data = [";
//  for (size_t i = 0; i < BURST_SIZE-1; i++) {
//    std::cout << std::dec << std::setw(1) << static_cast<int>(a_burst_binary[i]) << ",";
//  }
//  std::cout << std::dec << std::setw(1) << static_cast<int>(a_burst_binary[BURST_SIZE-1]) << "]" << std::endl;

}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
void passive_receiver::dbg_dump_result(uint32_t burst_nr, const uint8_t *a_result, size_t a_res_len)
{
  std::cout << "Result[" << std::dec << burst_nr
      << "][ ";
  for (size_t i=0; i<a_res_len; i++) {
    std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(a_result[i]) << " ";
  }
  std::cout << "]" << std::endl;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
void passive_receiver::push_burst(const uint8_t *a_burst_binary)
{
  // Shift left to make space for current burst.
  for (size_t i=0; i<(m_nof_brsts-1); i++) {
    ::memcpy(reinterpret_cast<void *>(&m_bursts_u[m_bdata_size*i]),
        reinterpret_cast<const void *>(&m_bursts_u[m_bdata_size*(i+1)]),
        size_t(m_bdata_size));
  }

  // Copy last burst bits into buffer
  ::memcpy(reinterpret_cast<void *>(&m_bursts_u[3*m_bdata_size]),
      reinterpret_cast<const void *>(&a_burst_binary[TAIL_BITS]),
      size_t(DATA_BITS+STEALING_BIT));
  ::memcpy(reinterpret_cast<void *>(&m_bursts_u[3*m_bdata_size+(DATA_BITS+STEALING_BIT)]),
      reinterpret_cast<const void *>(&a_burst_binary[TAIL_BITS+DATA_BITS+STEALING_BIT+N_TRAIN_BITS]),
      size_t(DATA_BITS+STEALING_BIT));

}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
size_t passive_receiver::decode_burst(uint8_t* a_result)
{
//  for (size_t j=0; j<m_nof_brsts; j++) {
//    std::cout << "[" << j << "][";
//    for (size_t i=0; i<m_bdata_size; i++) {
//      std::cout << std::dec << std::setw(1) << static_cast<int>(m_bursts_u[j*m_bdata_size+i]);
//    }
//    std::cout << "]" << std::endl;
//  }

  size_t res = 0/*decode(m_bursts_u, m_bdata_size*m_nof_brsts, a_result)*/;

  return res;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
uint32_t passive_receiver::get_burst_start(gr_complex *a_burst_input, gr_complex *imp_response, float &a_corr_max)
{
  get_norm_chan_imp_resp(a_burst_input, imp_response, &a_corr_max, 0);

  float ts_max = a_corr_max;
  int ts_max_num = 0;

  for (int ss = 1; ss < TRAIN_SEQ_NUM; ss++) {
    get_norm_chan_imp_resp(a_burst_input, imp_response, &a_corr_max, ss);

    if (ts_max < a_corr_max) {
      ts_max = a_corr_max;
      ts_max_num = ss;
    }
  }

  return get_norm_chan_imp_resp(a_burst_input, imp_response, &a_corr_max, ts_max_num);
}
//----------------------------------------------------------------------------//
