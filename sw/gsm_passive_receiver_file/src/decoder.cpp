/* -*- c++ -*- */
/*
 * @file
 * @author (C) 2014 by Piotr Krysik <ptrkrysik@gmail.com>
 * @section LICENSE
 *
 * Gr-gsm is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * Gr-gsm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with gr-gsm; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include <cstddef>
#include <cstdint>

#include <string.h>

#include "decoder.h"

#define DATA_BYTES 23

//----------------------------------------------------------------------------//
static int ubits2sbits(const ubit_t *ubits, sbit_t *sbits, int count)
{
  int i;

  for (i = 0; i < count; i++) {
    if (*ubits == 0x23) {
      ubits++;
      sbits++;
      continue;
    }
    if ((*ubits++) & 1)
      *sbits++ = -127;
    else
      *sbits++ = 127;
  }

  return count;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
size_t decode(uint8_t *bursts_u, uint32_t bursts_size, int *a_errors, int *a_bits, uint8_t *result)
{

  sbit_t bursts_s[GSM0503_GPRS_BURSTS_NBITS];

  //convert to soft bits
  ubits2sbits(bursts_u, bursts_s, bursts_size);
  //decode
  if (gsm0503_xcch_decode(result, bursts_s, a_errors, a_bits) != -1) {

    return DATA_BYTES;
  }
  return 0;
}
//----------------------------------------------------------------------------//

