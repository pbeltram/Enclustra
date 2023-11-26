// See LICENSE ane README.md files.

#include <algorithm>
#include <memory>
#include <cstring>
#include <string>
#include <stdexcept>

#include <assert.h>

#include "receiver_worker.h"

//----------------------------------------------------------------------------//
template<typename ... Args>
std::string string_format( const std::string& format, Args ... args )
{
  int size_s = std::snprintf( nullptr, 0, format.c_str(), args ... ) + 1;
  if( size_s <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
  auto size = static_cast<size_t>( size_s );
  std::unique_ptr<char[]> buf( new char[ size ] );
  std::snprintf( buf.get(), size, format.c_str(), args ... );
  return std::string( buf.get(), buf.get() + size - 1 );
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
receiver_worker::receiver_worker(std::atomic<bool> &a_abort,
    uint32_t a_nof_a, std::string &a_finp,
    uint32_t a_decimate, uint32_t a_sampling_ratio, double a_mva_threshold,
    bool a_write) :
  m_abort(a_abort),
  m_decimate(a_decimate),
  m_cplx_readed(0),
  m_buffer_wr_ptr(nullptr),
  m_total_bytes(0),
  m_buffer_rd_cplx_ptr(nullptr),
  m_nof_cplx_read_size(a_nof_a*sizeof(gr_complex)),
  m_buffer_rd_16bit_ptr(nullptr),
  m_nof_16bit_read_size(a_nof_a*sizeof(std::complex<int16_t >)),
  m_dec(0),
  m_osr(4 * a_sampling_ratio),
  m_write(a_write)
{

  const float gsm_symb_rate = GSM_SYMBOL_RATE;
  const float samp_rate_out = gsm_symb_rate * m_osr;

  m_preceiver = new passive_receiver(m_osr, a_mva_threshold,
      std::bind(&receiver_worker::get_sample, this, std::placeholders::_1));
  m_buffer_wr_ptr = new uint8_t[std::max(m_nof_cplx_read_size, m_nof_16bit_read_size)];

  m_ifs.open(a_finp.c_str(), std::ios::binary | std::ios::in);
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
receiver_worker::~receiver_worker()
{
  m_ifs.close();

  delete m_preceiver;
  delete [] m_buffer_wr_ptr;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
bool receiver_worker::get_raw_cplx_sample(gr_complex &a_sample)
{
  if (m_abort.load() == true)
    return false;

  if (m_cplx_readed == 0) {
    // Refill read buffer.
    if (m_ifs.peek() == EOF) {
      m_abort = true;
      return false;
    }
    m_ifs.read(reinterpret_cast<char *>(m_buffer_wr_ptr), m_nof_cplx_read_size);
    m_cplx_readed = m_ifs.gcount()/sizeof(gr_complex);
    if (m_cplx_readed == 0) {
      m_abort = true;
      return false;
    }
    m_buffer_rd_cplx_ptr = reinterpret_cast<gr_complex *>(m_buffer_wr_ptr);
    m_total_bytes += m_nof_cplx_read_size;
  }

  a_sample = *m_buffer_rd_cplx_ptr;

  ++m_buffer_rd_cplx_ptr;
  --m_cplx_readed;

  return true;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
bool receiver_worker::get_raw_16bit_sample(gr_complex &a_sample)
{
  if (m_abort.load() == true)
    return false;

  if (m_cplx_readed == 0) {
    // Refill read buffer.
    if (m_ifs.peek() == EOF) {
      m_abort = true;
      return false;
    }
    m_ifs.read(reinterpret_cast<char *>(m_buffer_wr_ptr), m_nof_16bit_read_size);
    m_cplx_readed = m_ifs.gcount()/sizeof(std::complex<int16_t >);
    if (m_cplx_readed == 0) {
      m_abort = true;
      return false;
    }
    m_buffer_rd_16bit_ptr = reinterpret_cast<std::complex<int16_t > *>(m_buffer_wr_ptr);
    m_total_bytes += m_nof_16bit_read_size;
  }

  std::complex<int16_t > val = *m_buffer_rd_16bit_ptr;
  gr_complex iq(val.real(), val.imag());

  a_sample.real(m_buffer_rd_16bit_ptr->real() / 2048.0f);
  a_sample.imag(m_buffer_rd_16bit_ptr->imag() / 2048.0f);

  ++m_buffer_rd_16bit_ptr;
  --m_cplx_readed;

  return true;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
bool receiver_worker::get_sample(gr_complex &a_sample)
{
  bool bRet;
  gr_complex avg_sample(0, 0);
  gr_complex sample(0, 0);

  while ((m_dec++ < m_decimate) && ((bRet = get_raw_cplx_sample(sample)/*get_raw_16bit_sample(sample)*/) == true)) {
    avg_sample += sample;
  }
  if (bRet == false)
    return false;

  m_dec = 0;
  avg_sample /= float(m_decimate);

  a_sample = avg_sample;

  return bRet;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
void receiver_worker::run()
{
  gr_complex imp_resp[CHAN_IMP_RESP_LENGTH * m_osr];
  gr_complex filtered_burst[BURST_SIZE];
  float normal_corr;
  uint8_t output_binary[BURST_SIZE];
  uint8_t result[128];
  gr_complex* burst_buffer = m_preceiver->get_burst_buffer();
  size_t burst_buffer_size = m_preceiver->get_burst_buff_size();

  while (m_abort.load() == false) {
    bool bRet = m_preceiver->work();
    if (bRet == true) {
      normal_corr = -1e6;
      uint32_t burst_buff_start = m_preceiver->get_burst_start(burst_buffer, imp_resp, normal_corr);

      m_preceiver->detect_burst(burst_buffer, imp_resp, burst_buff_start, filtered_burst, output_binary);
      m_preceiver->dbg_dump_burst(m_preceiver->get_burst_nr(), burst_buff_start, output_binary, normal_corr);

      // Save burst
      if (m_write) {
        std::ofstream ofs;
        std::string fname;

        fname = string_format("./burst_%03d.cf64", m_preceiver->get_burst_nr());
        ofs.open(fname.c_str(), std::ios::binary | std::ios::out | std::ios::trunc);
        ofs.write(reinterpret_cast<char *>(burst_buffer), burst_buffer_size * sizeof(gr_complex));
        ofs.close();

        fname = string_format("./burst_%03d-filtered.cf64", m_preceiver->get_burst_nr());
        ofs.open(fname.c_str(), std::ios::binary | std::ios::out | std::ios::trunc);
        ofs.write(reinterpret_cast<char *>(filtered_burst), BURST_SIZE * sizeof(gr_complex));
        ofs.close();

        fname = string_format("./burst_%03d-impresp.cf64", m_preceiver->get_burst_nr());
        ofs.open(fname.c_str(), std::ios::binary | std::ios::out | std::ios::trunc);
        ofs.write(reinterpret_cast<char *>(imp_resp), (CHAN_IMP_RESP_LENGTH * m_osr) * sizeof(gr_complex));
        ofs.close();

      }

      m_preceiver->push_burst(output_binary);
//      ::memset(reinterpret_cast<void *>(result), 0, sizeof(result));
//      size_t res_len = m_preceiver->decode_burst(result);
//      m_preceiver->dbg_dump_result(m_preceiver->get_burst_nr(), result, 26/*res_len*/);
    }
  }
}
//----------------------------------------------------------------------------//
