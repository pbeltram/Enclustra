/*
 * receiver_worker.cpp
 *
 *  Created on: Oct 7, 2023
 *      Author: primozb
 */

#include <algorithm>
#include <memory>
#include <cstring>
#include <string>
#include <stdexcept>
#include <iomanip>
#include <assert.h>
#include <errno.h>

#include <arpa/inet.h> // htons, inet_addr
#include <sys/socket.h> // socket, sendto
#include <unistd.h> // close

#include "util.h"
#include "gsm_constants.h"
#include "gr_firdes.h"
#include "gsmbox.h"
#include "gsmtap.h"
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
    SignalDataPtr_type &a_sig_ch,
    double a_mva_threshold, float a_freq_offs, uint16_t a_arfcn, bool a_uplink,
    bool a_write, bool a_write_burst, std::string &a_udp_dest) :
  m_abort(a_abort),
  m_cplx_readed(0),
  m_sig_ch(a_sig_ch),
  m_iq_dat(nullptr),
  m_iq_end(nullptr),
  m_inc(0),
  m_osr(8),
  m_write(a_write),
  m_write_burst(a_write_burst),
  m_bdata_size((DATA_BITS+STEALING_BIT)*2),
  m_nof_brsts(4),
  m_bursts_u(nullptr),
  m_freq_offs(a_freq_offs),
  m_bsc(1),
  m_arfcn(a_arfcn),
  m_uplink(a_uplink),
  m_send_udp(false)
{

  const float gsm_symb_rate = GSM_SYMBOL_RATE;
  const float samp_rate = gsm_symb_rate * m_osr;
  m_lpfir_len = create_lpfir_filter(samp_rate, 0, nullptr);
  m_lpfir_coeffs = new float[m_lpfir_len];
  m_lpfir_len = create_lpfir_filter(samp_rate, m_lpfir_len, m_lpfir_coeffs);
  m_lpfir_flush = m_lpfir_len;

  m_preceiver = new passive_receiver(m_osr, a_mva_threshold,
      std::bind(&receiver_worker::get_sample, this, std::placeholders::_1));

  if (m_write == true) {
    m_ofs.open("./iq_16bit.dat", std::ios::binary | std::ios::out | std::ios::trunc);
  }

  m_bursts_u = new uint8_t[m_bdata_size * m_nof_brsts];

  m_udp_sock = ::socket(AF_INET, SOCK_DGRAM, 0);
  m_udp_dest.sin_family = AF_INET;
  m_udp_dest.sin_port = ::htons(GSMTAP_UDP_PORT);
  m_udp_dest.sin_addr.s_addr = ::inet_addr(a_udp_dest.c_str());
  m_send_udp = true;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
receiver_worker::~receiver_worker()
{
  ::close(m_udp_sock);
  if (m_write == true) {
    m_ofs.close();
  }

  delete m_preceiver;
  delete [] m_bursts_u;
  delete [] m_lpfir_coeffs;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
void receiver_worker::send_udp(uint32_t a_burst_no, uint16_t a_arfcn, bool a_uplink, int8_t a_sig_dbm,
    uint32_t a_res_len, uint8_t *a_result)
{
  uint8_t buf[sizeof(gsmtap_hdr) + a_res_len];
  ::memset(reinterpret_cast<void *>(buf), 0, sizeof(buf));

  struct gsmtap_hdr *tap_header = (struct gsmtap_hdr *) buf;
  uint8_t *result = buf + sizeof(gsmtap_hdr);

  tap_header->version = GSMTAP_VERSION;
  tap_header->hdr_len = sizeof(gsmtap_hdr)/4;
  tap_header->type = GSMTAP_TYPE_UM;
  tap_header->sub_type = GSMTAP_BURST_NORMAL;

  tap_header->frame_number = ::htobe32(a_burst_no);
  tap_header->timeslot = 0;

  tap_header->arfcn = ::htobe16(a_arfcn | (a_uplink ? GSMTAP_ARFCN_F_UPLINK:0));

  tap_header->signal_dbm = a_sig_dbm;
  tap_header->snr_db = 0;

  ::memcpy(result, a_result, a_res_len);

  ::sendto(m_udp_sock, buf, sizeof(buf), 0,
      reinterpret_cast<sockaddr*>(&m_udp_dest), sizeof(m_udp_dest));

  return;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
void receiver_worker::dbg_dump_burst(uint32_t burst_nr, uint32_t a_burst_start_pos,
    uint32_t a_burst_start, const uint8_t *a_burst_binary, uint32_t a_bsc, float a_corr_max)
{
  std::cout << "No=" << std::dec<< std::setfill(' ') << std::setw(3) << burst_nr
      << " Corr=" << std::fixed << std::setfill(' ') << std::setw(6) << std::setprecision(3) << a_corr_max
      << " Start=" << std::dec << std::setw(10) << a_burst_start_pos
      << " buff_off=" << a_burst_start
      << " bsc=" << a_bsc
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
void receiver_worker::dbg_dump_result(uint32_t burst_nr, const uint8_t *a_result, size_t a_res_len)
{
  std::cout << "Result[" << std::dec << std::setfill(' ') << std::setw(3) << burst_nr
      << "][ ";
  for (size_t i=0; i<a_res_len; i++) {
    std::cout << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(a_result[i]) << " ";
  }
  std::cout << "]" << std::endl;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
void receiver_worker::push_burst(const uint8_t *a_burst_binary)
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
bool receiver_worker::get_raw_16bit_sample(gr_complex &a_sample)
{
  std::lock_guard<std::mutex> lock(m_sig_ch->get_mutex());

  if (m_abort.load() == true)
    return false;

  if (m_iq_dat == m_iq_end) {
    assert((m_cplx_readed == 0) && ":Initial nof samples.");
    // Refill read buffer.
    m_cplx_readed = m_sig_ch->refill()/m_sig_ch->sample_size();

//    std::cout << "INFO: Refill cnt=" << std::dec << m_sig_ch->get_refill_cnt()
//              << " Nof samples=" << std::dec << m_sig_ch->get_nofsamples()
//              << " Nof readed=" << std::dec << m_cplx_readed
//              << " Sample size=" << std::dec << m_sig_ch->sample_size()
//              << std::endl;

    if (m_cplx_readed > 0) {
      m_iq_dat = m_sig_ch->get_iiovalues();
      m_iq_end = m_sig_ch->get_rxbuf_end();
      m_inc = m_sig_ch->get_rxbuf_inc();

      if (m_write) m_ofs.write(reinterpret_cast<char *>(m_iq_dat), m_cplx_readed * m_sig_ch->sample_size());
    }
    else {
      std::cout << "ERROR: Fill IIO rx buffer failed. errno=" << errno
                << "-" << get_iio_strerror(errno).c_str()
                << std::endl;

      m_iq_dat = nullptr;
      m_iq_end = static_cast<const SignalData::iiovalues_t *>(nullptr);
      m_abort = true;
      return false;
    }
  }

  if (m_iq_dat < m_iq_end) {
    SignalData::iiovalues_t val = *m_iq_dat;
    gr_complex iq(val.real(), val.imag());

    a_sample.real(m_iq_dat->real() / 2048.0f);
    a_sample.imag(m_iq_dat->imag() / 2048.0f);

    ++m_iq_dat;
    --m_cplx_readed;

    return true;
  }

  assert((1 == 0) && ":Should not happen.");
  return false;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
bool receiver_worker::get_sample(gr_complex &a_sample)
{
  return get_raw_16bit_sample(a_sample);
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
void receiver_worker::worker_thread_func()
{
  {
    std::lock_guard<std::mutex> lock_started(m_worker_thread_started_mutex);
    m_worker_thread_started = true;
  }
  m_cv_worker_thread_started.notify_one();

  while (m_abort.load() == false) {
    bool bRet = m_preceiver->work();
    if (bRet == true) {
      burst_data_t burst_data = std::make_tuple(m_preceiver->get_burst_buffer(),
          m_preceiver->get_burst_nr(), m_preceiver->get_burst_start_pos());

      #if !defined(NDEBUG)
      std::cout
          << "INFO: Receiver worker thread Burst:"
          << "No=" << std::dec << std::setw(3) << std::get<1>(burst_data)
          << " Start=" << std::dec << std::setw(10) << std::get<2>(burst_data)
          << std::endl;
      #endif

      {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_burst_data_list.push_back(burst_data);
        m_get_burst = true;
      }
      m_cv_worker_wake.notify_one();
    }
  }

  {
    std::lock_guard<std::mutex> lock(m_mutex);
    // m_abort == true
  }
  m_cv_worker_wake.notify_all();

  std::cout << "INFO: Receiver worker thread Done." << std::endl;

  return;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
void receiver_worker::run()
{
  gr_complex imp_resp_bsc[CHAN_IMP_RESP_LENGTH * m_osr];
  gr_complex filtered_burst[BURST_SIZE];
  float normal_corr;
  uint8_t output_binary[BURST_SIZE];
  uint8_t result[128];
  const size_t burst_buffer_size = m_preceiver->get_burst_buff_size();
  gr_complex burst_fir[burst_buffer_size+m_lpfir_len];
  gr_complex train_seq[TRAIN_SEQ_NUM][N_TRAIN_BITS];
  burst_data_t burst_data;

  for (uint32_t bsc=0; bsc<TRAIN_SEQ_NUM; bsc++) {
    get_train_seq_iq(bsc, train_seq[bsc]);
  }

  std::unique_lock<std::mutex> lock_started(m_worker_thread_started_mutex);
  m_worker_thread_started = false;
  m_worker_thread = std::thread(std::bind(&receiver_worker::worker_thread_func, this));
  // wait that thread starts before other operations can be done
  m_cv_worker_thread_started.wait(lock_started, [this] () { return m_worker_thread_started.load(); });

  do {
    {
      std::unique_lock<std::mutex> lock(m_mutex);
      m_cv_worker_wake.wait(lock, [this] () { return m_abort.load() || m_get_burst.load(); });
    }

    if (m_get_burst.load()) {
      while ((m_burst_data_list.empty() == false) && (m_abort.load() == false)) {
        {
          std::lock_guard<std::mutex> lock(m_mutex);
          burst_data = m_burst_data_list.front();
          m_burst_data_list.pop_front();
        }
        gr_complex* burst_buffer = std:: get<0>(burst_data);
        uint32_t burst_no = std::get<1>(burst_data);
        uint32_t burst_start_pos = std::get<2>(burst_data);

        #if !defined(NDEBUG)
        std::cout
            << "INFO: Burst:"
            << "No=" << std::dec << std::setw(3) << burst_no
            << " Start=" << std::dec << std::setw(10) << burst_start_pos
            << std::endl;
        #endif

        if (m_write_burst) {
          std::ofstream ofs;
          std::string fname;

          fname = string_format("./burst_%03d.cf64", m_preceiver->get_burst_nr());
          ofs.open(fname.c_str(), std::ios::binary | std::ios::out | std::ios::trunc);
          ofs.write(reinterpret_cast<char *>(burst_buffer), burst_buffer_size * sizeof(gr_complex));
          ofs.close();
        }

        double phase_inc = -2*M_PI*m_freq_offs/(m_osr * GSM_SYMBOL_RATE);
        uint32_t resp_len = filter_data(m_lpfir_len, m_lpfir_coeffs, phase_inc, burst_buffer_size, burst_buffer, burst_fir);
        gr_complex* burst = burst_fir + m_lpfir_len/2;

        m_bsc = 0;
        float corr_max_bsc = 0.0f;
        uint32_t burst_offset_bsc = 0;

        for (uint32_t bsc=0; bsc<TRAIN_SEQ_NUM; bsc++) {
          uint32_t burst_offset = get_burst_offset(m_osr, train_seq[bsc], burst);
          gr_complex imp_resp[CHAN_IMP_RESP_LENGTH * m_osr];
          get_imp_resp(m_osr, train_seq[bsc], (burst + burst_offset), imp_resp);
          float corr = get_imp_resp_max(m_osr, imp_resp);
          if (corr > corr_max_bsc) {
            corr_max_bsc = corr;
            m_bsc = bsc;
            burst_offset_bsc = burst_offset;
            ::memcpy(reinterpret_cast<void *>(imp_resp_bsc), reinterpret_cast<const void *>(imp_resp),
                    size_t((CHAN_IMP_RESP_LENGTH*m_osr)*sizeof(gr_complex)));
          }
        }

        burst = burst + burst_offset_bsc;
        mafi_burst(m_osr, imp_resp_bsc, burst, filtered_burst);

        demod_burst(m_osr, imp_resp_bsc, filtered_burst, output_binary);
        dbg_dump_burst(burst_no, burst_start_pos, burst_offset_bsc, output_binary, m_bsc, corr_max_bsc);
        push_burst(output_binary);

        ::memset(reinterpret_cast<void *>(result), 0, sizeof(result));
        uint32_t res_len = decode_bursts(m_bursts_u, (m_bdata_size * m_nof_brsts), sizeof(result), result);
        if (res_len) {
          dbg_dump_result(burst_no, result, 26);
          if (m_send_udp) {
            float signal_dbm = round(10 * log10((corr_max_bsc * corr_max_bsc) / 50));
            int8_t sig_dbm = static_cast<int8_t>(signal_dbm);

            send_udp(burst_no, m_arfcn, m_uplink, sig_dbm, res_len, result);
          }
        }

        delete [] burst_buffer;
      }

      {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_get_burst = (m_burst_data_list.empty() == false);
      }
    }
  } while (m_abort.load() == false);

  if (m_worker_thread.joinable())
    m_worker_thread.join();

  // Free allocated memory.
  uint32_t bursts_deleted(0);
  {
    std::lock_guard<std::mutex> lock(m_mutex);
    while (m_burst_data_list.empty() == false) {
      burst_data = m_burst_data_list.front();
      m_burst_data_list.pop_front();
      gr_complex* burst_buffer = std:: get<0>(burst_data);
      uint32_t burst_no = std::get<1>(burst_data);
      uint32_t burst_start_pos = std::get<2>(burst_data);
      delete [] burst_buffer;
      ++bursts_deleted;
    }
  }

  std::cout << "INFO: Receiver Done:"
      << " Not processed bursts=" << std::dec << bursts_deleted
      << std::endl;

  return;
}
//----------------------------------------------------------------------------//
