/*
 * signaldata.h
 *
 *  Created on: Oct 7, 2023
 *      Author: primozb
 */

#ifndef _SIGNAL_DATA_H_
#define _SIGNAL_DATA_H_

#include <cstddef>
#include <complex>
#include <mutex>
#include <memory>

#include <iio.h>

//----------------------------------------------------------------------------//
class SignalData
{
public:
  typedef std::complex<int16_t > iiovalues_t;

  SignalData(iio_context * a_iio_ctx = NULL, const char * a_iio_dev = NULL, size_t a_nofsamples = 0);

  ~SignalData();

  bool is_valid() { return m_valid; }
  std::mutex& get_mutex() { return m_mutex; }
  const std::string& get_name() { return m_name; };

  ssize_t refill();
  iiovalues_t * get_iiovalues();
  size_t get_refill_cnt() { return m_refill_cnt; };
  size_t get_nofsamples() { return m_nofsamples; };
  size_t sample_size();
  size_t get_rxbuf_inc() { return m_rxbuf_inc; };
  const iiovalues_t* get_rxbuf_end() { return m_rxbuf_end; };

private:
  bool m_valid;
  size_t m_nofsamples;

  iio_device  *m_iio_dev;

  iio_channel *m_iio_chni;
  iio_channel *m_iio_chnq;

  size_t       m_rxbuf_inc;
  iiovalues_t* m_rxbuf_end;
  iio_buffer  *m_iio_rxbuf;

  std::mutex m_mutex;
  std::string m_name;

  size_t m_refill_cnt;
};
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
typedef std::shared_ptr<SignalData> SignalDataPtr_type;
//----------------------------------------------------------------------------//

#endif //_SIGNAL_DATA_H_
