/*
 * passive_receiver.cpp
 *
 *  Created on: Oct 7, 2023
 *      Author: primozb
 */

#include <cmath>
#include <algorithm>
#include <string.h>
#include <iostream>
#include <numeric>
#include <vector>

#include <assert.h>

#include "gsm_constants.h"
#include "gr_complex.h"
#include "passive_receiver.h"

//----------------------------------------------------------------------------//
passive_receiver::passive_receiver(int a_osr, const double a_mwa_threshold, callback_funct_t a_get_sample_fptr) :
  m_osr(a_osr),
  m_counter(0),
  m_burst_start_pos(0),
  m_burst_buffer(nullptr),
  m_burst_nr(0),
  m_get_sample_fptr(a_get_sample_fptr),
  m_mwa_threshold(a_mwa_threshold)
{

  m_burst_buff_size = ((BURST_SIZE+GUARD_BITS)*m_osr);

  m_mva_len = m_burst_buff_size;
  m_mva_buffer = new std::list<double>(m_mva_len, double(0));
  m_mva_sum = 0;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
passive_receiver::~passive_receiver()
{
  delete m_mva_buffer;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
bool passive_receiver::work()
{
  //TODO: Move filtering and find burst to FPGA.
  m_burst_buffer = new gr_complex[m_burst_buff_size];
  if (!find_burst()) {
    delete [] m_burst_buffer;
    return false;
  }

  return true;
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

  enum states { search, find_max, search_fail } burst_search_state;

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
          m_burst_start_pos = (m_counter-2) - m_burst_buff_size;
          m_burst_nr++;
          end = true;
          result = true;
          // Clear MVA.
          delete m_mva_buffer;
          m_mva_buffer = new std::list<double>(m_mva_len, double(0));
          m_mva_sum = 0;
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

