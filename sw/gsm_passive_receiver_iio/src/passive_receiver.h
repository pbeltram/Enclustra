/*
 * passive_receiver.h
 *
 *  Created on: Oct 7, 2023
 *      Author: primozb
 */

#ifndef INCLUDED_PASSIVE_RECEIVER_H
#define INCLUDED_PASSIVE_RECEIVER_H

#include <list>
#include <functional>

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
uint32_t get_burst_start_pos() { return m_burst_start_pos; }
//----------------------------------------------------------------------------//

private:

//----------------------------------------------------------------------------//
void update_burst_buffer(const gr_complex &a_sample);
bool get_sample(gr_complex &a_sample);

bool find_burst();

double update_mva(double a_val);
//----------------------------------------------------------------------------//

private:

size_t m_mva_len;
std::list<double>* m_mva_buffer;
double m_mva_sum;

const uint32_t m_osr;

uint32_t m_counter;

uint32_t m_burst_start_pos;

gr_complex *m_burst_buffer;
size_t m_burst_buff_size;

uint32_t m_burst_nr;

callback_funct_t m_get_sample_fptr;
const double m_mwa_threshold;

};
//----------------------------------------------------------------------------//

#endif // INCLUDED_PASSIVE_RECEIVER_H

