// See LICENSE ane README.md files.

#ifndef INCLUDED_RECEIVER_WORKER_H
#define INCLUDED_RECEIVER_WORKER_H

#include <cstdint>
#include <cstddef>
#include <iostream>
#include <fstream>
#include <atomic>

#include "gr_complex.h"
#include "passive_receiver.h"

//----------------------------------------------------------------------------//
class receiver_worker
{

public:

receiver_worker(std::atomic<bool> &a_abort,
    uint32_t a_nof_a, std::string &a_finp,
    uint32_t a_decimate, uint32_t a_sampling_ratio,  double a_mva_threshols,
    bool a_write);
~receiver_worker();

bool get_sample(gr_complex &a_sample);
void run();

protected:

bool get_raw_cplx_sample(gr_complex &a_sample);
bool get_raw_16bit_sample(gr_complex &a_sample);

private:

std::atomic<bool> &m_abort;
uint32_t m_decimate;

passive_receiver* m_preceiver;

ssize_t m_cplx_readed;
std::ifstream m_ifs;
uint8_t* m_buffer_wr_ptr;
size_t m_total_bytes;

gr_complex* m_buffer_rd_cplx_ptr;
size_t m_nof_cplx_read_size;
size_t m_dec;

std::complex<int16_t >* m_buffer_rd_16bit_ptr;
size_t m_nof_16bit_read_size;
uint32_t m_osr;

bool m_write;

};
//----------------------------------------------------------------------------//

#endif // INCLUDED_RECEIVER_WORKER_H

