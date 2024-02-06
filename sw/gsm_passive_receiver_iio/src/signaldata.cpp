/*
 * signaldata.cpp
 *
 *  Created on: Oct 7, 2023
 *      Author: primozb
 */

#include <iostream>

#include <assert.h>
#include <errno.h>
#include <string.h>

#include "util.h"
#include "signaldata.h"

//----------------------------------------------------------------------------//
SignalData::SignalData(iio_context * a_iio_ctx, const char * a_iio_dev, size_t a_nofsamples) :
    m_valid(false),
    m_nofsamples(a_nofsamples),
    m_iio_dev(nullptr),
    m_iio_chni(nullptr),
    m_iio_chnq(nullptr),
    m_rxbuf_inc(0),
    m_rxbuf_end(nullptr),
    m_iio_rxbuf(nullptr),
    m_name(a_iio_dev),
    m_refill_cnt(0)
{
  m_iio_dev = ::iio_context_find_device(a_iio_ctx, a_iio_dev);
  if (m_iio_dev == nullptr) {
    std::cout << "ERROR: Find IIO device " << a_iio_dev
              << " failed. errno=" << errno
              << "-" << get_iio_strerror(errno)
              << std::endl;
    return;
  }

  int res = ::iio_device_set_kernel_buffers_count(m_iio_dev, 16);
  assert((res == 0) && ":IIO iio_device_set_kernel_buffers_count failed.");

  m_iio_chni = ::iio_device_find_channel(m_iio_dev, "voltage0", false);
  if (m_iio_chni == nullptr) {
    std::cout << "ERROR: Find IIO channel voltage0 failed. errno=" << errno
              << "-" << get_iio_strerror(errno)
              << std::endl;
    return;
  }
  m_iio_chnq = ::iio_device_find_channel(m_iio_dev, "voltage1", false);
  if (m_iio_chni == nullptr) {
    std::cout << "ERROR: Find IIO channel voltage1 failed. errno=" << errno
              << "-" << get_iio_strerror(errno)
              << std::endl;
    return;
  }

  std::cout << "INFO: Enable IIO: " << m_name
            << " input streaming channels nofs: " << a_nofsamples
            << std::endl;
  ::iio_channel_enable(m_iio_chni);
  ::iio_channel_enable(m_iio_chnq);

  //std::cout << "DEBUG: Creating non-cyclic RX IIO buffer samples: " << m_nofsamples
  //          << std::endl;
  m_iio_rxbuf = ::iio_device_create_buffer(m_iio_dev, m_nofsamples, false);
  if (m_iio_rxbuf == nullptr) {
    std::cout << "ERROR: Create IIO: " << m_name
              << " RX buffer samples " << m_nofsamples
              << " failed. errno=" << errno
              << "-" << get_iio_strerror(errno)
              << std::endl;
    return;
  }
//  int res = ::iio_buffer_set_blocking_mode(m_iio_rxbuf, false);
//  assert((res == 0) && ":IIO iio_buffer_set_blocking_mode failed.");

  m_rxbuf_inc = size_t(::iio_buffer_step(m_iio_rxbuf));
  m_rxbuf_end = static_cast<iiovalues_t * >(::iio_buffer_end(m_iio_rxbuf));

  m_valid = true;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
SignalData::~SignalData()
{
  if ((m_iio_chni != nullptr) && (::iio_channel_is_enabled(m_iio_chni) == true)) {
    ::iio_channel_disable(m_iio_chni);
  }
  if ((m_iio_chnq != nullptr) && (::iio_channel_is_enabled(m_iio_chnq) == true)) {
    ::iio_channel_disable(m_iio_chnq);
  }

  if (m_iio_rxbuf != nullptr) {
    ::iio_buffer_destroy(m_iio_rxbuf);
  }

  std::cout << "INFO: Destructor of: " << m_name << " SignalData done."
            << std::endl;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
ssize_t SignalData::refill()
{
  ssize_t nbytes_rx = ::iio_buffer_refill(m_iio_rxbuf);
  if (nbytes_rx < 0) {
    std::cout << "ERROR: Reffil IIO: " << m_name
              << " RX bytes=" << nbytes_rx
              << " failed. errno=" << errno
              << "-" << ::strerror(errno)
              << " IIO error: " << get_iio_strerror(-nbytes_rx)
              << std::endl;
    return 0;
  }

  ++m_refill_cnt;

  //std::cout << "DEBUG: Re-fill IIO buffer RX=" << nbytes_rx << "bytes. "
  //          << "Samples=" << nbytes_rx/sample_size()
  //          << std::endl;

  return nbytes_rx;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
size_t SignalData::sample_size()
{
  if (m_iio_dev == nullptr) return 0;
  else return ::iio_device_get_sample_size(m_iio_dev);
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
SignalData::iiovalues_t* SignalData::get_iiovalues()
{
  return static_cast<iiovalues_t * >(::iio_buffer_first(m_iio_rxbuf, m_iio_chni));
}
//----------------------------------------------------------------------------//
