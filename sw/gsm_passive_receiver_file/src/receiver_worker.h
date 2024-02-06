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

#include "gr_complex.h"
#include "passive_receiver.h"

//----------------------------------------------------------------------------//
class receiver_worker
{
typedef std::tuple<gr_complex*,  uint32_t , uint32_t > burst_data_t;

public:

//----------------------------------------------------------------------------//
receiver_worker(std::atomic<bool> &a_abort, uint32_t a_nof_a, std::string &a_finp,
    double a_mva_threshols, float a_freq_offs,
    bool a_write_burst, std::string &a_udp_dest);
~receiver_worker();

bool get_sample(gr_complex &a_sample);
void push_burst(const uint8_t *a_burst_binary);

void run();
void worker_thread_func();

void dbg_dump_burst(uint32_t burst_nr, uint32_t a_burst_start_pos,
    uint32_t a_burst_start, const uint8_t *a_burst_binary, uint32_t a_bsc, float a_corr_max);
void dbg_dump_result(uint32_t burst_nr, const uint8_t *a_result, size_t a_res_len);

void send_udp(uint32_t a_burst_no, uint16_t a_arfcn, bool a_uplink, int8_t a_sig_dbm,
    uint32_t a_res_len, uint8_t *a_result);
//----------------------------------------------------------------------------//

protected:

//----------------------------------------------------------------------------//
bool get_raw_cplx_sample(gr_complex &a_sample);
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

passive_receiver* m_preceiver;
float* m_lpfir_coeffs;
size_t m_lpfir_len;
size_t m_lpfir_flush;

ssize_t m_cplx_readed;
std::ifstream m_ifs;
uint8_t* m_buffer_wr_ptr;
size_t m_total_bytes;

gr_complex* m_buffer_rd_cplx_ptr;
size_t m_nof_cplx_read_size;

std::complex<int16_t >* m_buffer_rd_16bit_ptr;
size_t m_nof_16bit_read_size;
uint32_t m_osr;

std::string m_ifname;
std::ofstream m_ofs;
bool m_write_burst;

float m_freq_offs;
uint32_t m_bsc;

size_t   m_bdata_size; // Burst data bits (DATA_BITS+STEALING_BIT)*2=116
size_t   m_nof_brsts;  // 4 consecutive bursts
uint8_t* m_bursts_u; // Storage for 4 consecutive bursts bits (GSM0503_GPRS_BURSTS_NBITS)

std::list<burst_data_t> m_burst_data_list;

bool m_send_udp;
int m_udp_sock;
sockaddr_in m_udp_dest;

};
//----------------------------------------------------------------------------//

#endif // INCLUDED_RECEIVER_WORKER_H

