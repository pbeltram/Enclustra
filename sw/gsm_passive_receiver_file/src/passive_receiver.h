// See LICENSE ane README.md files.

#ifndef INCLUDED_PASSIVE_RECEIVER_H
#define INCLUDED_PASSIVE_RECEIVER_H

#include <vector>
#include <list>
#include <functional>

#include "gsm_constants.h"

/*----------------------------------------------------------------------------*/
typedef std::function<bool(gr_complex &)> callback_funct_t;
/*----------------------------------------------------------------------------*/

//----------------------------------------------------------------------------//
class passive_receiver
{
public:

//----------------------------------------------------------------------------//
passive_receiver(int a_osr, const double a_mwa_threshold, callback_funct_t a_get_sample_fptr);
~passive_receiver();
bool work();

gr_complex * get_burst_buffer() { return m_burst_buffer; }
size_t get_burst_buff_size() { return m_burst_buff_size; }
uint32_t get_burst_nr() { return m_burst_nr; }

uint32_t get_burst_start(gr_complex *a_burst_input, gr_complex *imp_response, float &a_corr_max);
// MLSE detection of a burst bits
void detect_burst(const gr_complex * input, gr_complex * chan_imp_resp, int burst_start, gr_complex * filtered_burst, unsigned char * output_binary);
void push_burst(const uint8_t *a_burst_binary);
size_t decode_burst(uint8_t* a_result);

void dbg_dump_burst(uint32_t burst_nr, uint32_t a_burst_start, const uint8_t *a_burst_binary, float corr_max);
void dbg_dump_result(uint32_t burst_nr, const uint8_t *a_result, size_t a_res_len);
//----------------------------------------------------------------------------//

private:

//----------------------------------------------------------------------------//
void update_burst_buffer(const gr_complex &a_sample);
bool get_sample(gr_complex &a_sample);

// Extracts channel impulse response from a SCH burst and computes first sample number of this burst
int get_sch_chan_imp_resp(const gr_complex *input, gr_complex * chan_imp_resp);

// Encodes differentially input bits and maps them into MSK states
void gmsk_mapper(const unsigned char * input, int nitems, gr_complex * gmsk_output, gr_complex start_point);

// Correlates MSK mapped sequence with input signal
gr_complex correlate_sequence(const gr_complex * sequence, int length, const gr_complex * input);

// Computes autocorrelation of input vector for positive arguments
inline void autocorrelation(const gr_complex * input, gr_complex * out, int nitems);

// Filters input signal through channel impulse response
inline void mafi(const gr_complex * input, int nitems, gr_complex * filter, int filter_length, gr_complex * output);

//  Extracts channel impulse response from a normal burst and computes first sample number of this burst
int get_norm_chan_imp_resp(const gr_complex *input, gr_complex * chan_imp_resp, float *corr_max, int bcc);

// State machine handlers
bool find_burst();

double update_mva(double a_val);
bool burst_search_handler();
//----------------------------------------------------------------------------//

private:

gr_complex m_norm_training_seq[TRAIN_SEQ_NUM][N_TRAIN_BITS]; // encoded training sequences of a normal and dummy burst

size_t m_mva_len;
std::list<double>* m_mva_buffer;
double m_mva_sum;

const uint32_t m_osr; // oversampling ratio

uint32_t m_counter; // Counts samples consumed by the receiver

uint32_t m_burst_start_pos; // position of the first sample of the burst
uint32_t m_burst_end_pos;

gr_complex *m_burst_buffer;
size_t m_burst_buff_size;

size_t   m_bdata_size; // Burst data bits (DATA_BITS+STEALING_BIT)*2=116
size_t   m_nof_brsts;  // 4 consecutive bursts
uint8_t* m_bursts_u; // Storage for 4 consecutive bursts bits (GSM0503_GPRS_BURSTS_NBITS)

uint32_t m_burst_nr;

callback_funct_t m_get_sample_fptr;
const double m_mwa_threshold;

};
//----------------------------------------------------------------------------//

#endif // INCLUDED_PASSIVE_RECEIVER_H

