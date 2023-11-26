/*
 * arfcn.cpp
 *
 *  Created on: Oct 7, 2023
 *      Author: primozb
 */

#include <vector>
#include <string>
#include <tuple>

//----------------------------------------------------------------------------//
typedef enum bands_key_ids {
  BAND_KEY_GSM900  = 0,
  BAND_KEY_DCS1800 = 1,
  BAND_KEY_GSM850  = 2,
  BAND_KEY_PCS1900 = 3,
  BAND_KEY_GSM450  = 4,
  BAND_KEY_GSM480  = 5,
  BAND_KEY_GSM_R   = 6,
  BAND_KEY_LAST    = 7, //Sentinel
} band_key_t;

typedef std::vector< std::tuple<int, int> > ranges_t;

typedef struct arfcn_band_tag {
  band_key_t band_key;
  std::string name;
  float f_start;
  float distance;
  ranges_t ranges;
} arfcn_band_t;

typedef std::vector<arfcn_band_t> arfcn_bands_t;
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
const float chan_spacing = 2e5;
const int arfcn_pcs = 0x8000;

const arfcn_bands_t arfcn_bands =
{
  { BAND_KEY_GSM900,  std::string("GSM900"),   880.2e6, 45e6, { std::tuple<int, int>(975, 1023), std::tuple<int, int>(0, 124) } },
  { BAND_KEY_DCS1800, std::string("DCS1800"), 1710.2e6, 95e6, { std::tuple<int, int>(512, 885) } },
  { BAND_KEY_GSM850,  std::string("GSM850"),   824.2e6, 45e6, { std::tuple<int, int>(128, 251) } },
  { BAND_KEY_PCS1900, std::string("PCS1900"), 1850.2e6, 80e6, { std::tuple<int, int>(512+arfcn_pcs, 810+arfcn_pcs) } },
  { BAND_KEY_GSM450,  std::string("GSM450"),   450.6e6, 10e6, { std::tuple<int, int>(259, 293) } },
  { BAND_KEY_GSM480,  std::string("GSM480"),   479.0e6, 10e6, { std::tuple<int, int>(306, 340) } },
  { BAND_KEY_GSM_R,   std::string("GSM-R"),    876.2e6, 45e6, { std::tuple<int, int>(955, 1023), std::tuple<int, int>(0, 124) } },
};
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
band_key_t arfcn2band(int a_arfcn)
{
  for (auto band_it = arfcn_bands.cbegin(); band_it != arfcn_bands.cend(); ++band_it) {
    for (auto it = band_it->ranges.cbegin(); it != band_it->ranges.cend(); ++it) {
      int arfcn_start = std::get<0>(*it);
      int arfcn_end = std::get<1>(*it);
      if ((arfcn_start <= a_arfcn) and (a_arfcn <= arfcn_end)) {
        return band_it->band_key;
      }
    }
  }
  return BAND_KEY_LAST;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
bool is_valid_arfcn(int a_arfcn)
{
  band_key_t band_key = arfcn2band(a_arfcn);
  return (band_key != BAND_KEY_LAST);
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
float arfcn2uplink(int a_arfcn)
{
  float fc = 0.0;
  band_key_t band_key = arfcn2band(a_arfcn);
  if (band_key != BAND_KEY_LAST) {
    const arfcn_band_t& band = arfcn_bands[band_key];
    float f_start = band.f_start;
    int arfcns_total = 0;
    for (auto it = band.ranges.cbegin(); it != band.ranges.cend(); ++it) {
      int arfcn_start = std::get<0>(*it);
      int arfcn_end = std::get<1>(*it);
      if ((arfcn_start <= a_arfcn) and (a_arfcn <= arfcn_end)) {
        fc = f_start + (chan_spacing * (a_arfcn - arfcn_start + arfcns_total));
        break;
      }
      arfcns_total = arfcn_end - arfcn_start + 1;
    }
  }
  return fc;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
float arfcn2downlink(int a_arfcn)
{
  float fc = 0.0;
  band_key_t band_key = arfcn2band(a_arfcn);
  if (band_key != BAND_KEY_LAST) {
    const arfcn_band_t& band = arfcn_bands[band_key];
    float distance = band.distance;
    fc = arfcn2uplink(a_arfcn) + distance;
  }
  return fc;
}
//----------------------------------------------------------------------------//
