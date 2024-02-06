/*
 * receiver_worker.h
 *
 *  Created on: Oct 7, 2023
 *      Author: primozb
 */

#ifndef INCLUDED_RECEIVER_WORKER_H
#define INCLUDED_RECEIVER_WORKER_H

#include <cstdint>
#include <cstddef>
#include <iostream>
#include <fstream>
#include <atomic>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <tuple>

#include <netinet/in.h> // sockaddr_in

#include "signaldata.h"
#include "gr_complex.h"
#include "passive_receiver.h"

//----------------------------------------------------------------------------//
class receiver_worker
{
typedef std::tuple<gr_complex*,  uint32_t , uint32_t > burst_data_t;

public:

receiver_worker(std::atomic<bool> &a_abort, SignalDataPtr_type &a_sig_ch,
    double a_mva_threshols, float a_freq_offs, uint16_t a_arfcn, bool a_uplink,
    bool a_write, bool a_write_burst, std::string &a_udp_dest);
~receiver_worker();

bool get_sample(gr_complex &a_sample);
void push_burst(const uint8_t *a_burst_binary);

void run();
void worker_thread_func();

void dbg_dump_burst(uint32_t burst_nr, uint32_t a_burst_start_pos,  uint32_t a_burst_start,
    const uint8_t *a_burst_binary, uint32_t a_bsc, float a_corr_max);
void dbg_dump_result(uint32_t burst_nr, const uint8_t *a_result, size_t a_res_len);

void send_udp(uint32_t a_burst_no, uint16_t a_arfcn, bool a_uplink, int8_t a_sig_dbm,
    uint32_t a_res_len, uint8_t *a_result);
//----------------------------------------------------------------------------//

protected:

bool get_raw_16bit_sample(gr_complex &a_sample);
//----------------------------------------------------------------------------//

std::mutex m_mutex;
std::thread m_worker_thread;

std::mutex m_worker_thread_started_mutex;
std::condition_variable m_cv_worker_thread_started;
std::atomic<bool> m_worker_thread_started{false};
std::condition_variable m_cv_worker_wake;

private:

std::atomic<bool> &m_abort;
std::atomic<bool> m_get_burst{false};

uint32_t m_arfcn_no;
bool m_arfcn_ul;

passive_receiver* m_preceiver;
float* m_lpfir_coeffs;
size_t m_lpfir_len;
size_t m_lpfir_flush;

ssize_t m_cplx_readed;
SignalDataPtr_type m_sig_ch;
SignalData::iiovalues_t* m_iq_dat;
const SignalData::iiovalues_t* m_iq_end;
size_t m_inc;

std::complex<int16_t >* m_buffer_rd_16bit_ptr;
size_t m_nof_16bit_read_size;
uint32_t m_osr;

std::ofstream m_ofs;
bool m_write;
bool m_write_burst;

float m_freq_offs;
uint32_t m_bsc;

size_t   m_bdata_size; // Burst data bits (DATA_BITS+STEALING_BIT)*2=116
size_t   m_nof_brsts;  // 4 consecutive bursts
uint8_t* m_bursts_u; // Storage for 4 consecutive bursts bits (GSM0503_GPRS_BURSTS_NBITS)

std::list<burst_data_t> m_burst_data_list;

uint16_t m_arfcn;
bool     m_uplink;

bool m_send_udp;
int m_udp_sock;
sockaddr_in m_udp_dest;

};
//----------------------------------------------------------------------------//

#endif // INCLUDED_RECEIVER_WORKER_H

