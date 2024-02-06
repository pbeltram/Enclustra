/*
 * util.cpp
 *
 *  Created on: Oct 7, 2023
 *      Author: primozb
 */

#include <iio.h>

#include "util.h"

//----------------------------------------------------------------------------//
const std::string get_iio_strerror(int a_ret)
{
  char errstr[1024];
  ::iio_strerror(a_ret, errstr, sizeof(errstr));

  return std::string(errstr);
}
//----------------------------------------------------------------------------//



