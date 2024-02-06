/*
 * ctrldata.cpp
 *
 *  Created on: Oct 7, 2023
 *      Author: primozb
 */

#include <iostream>
#include <chrono>
#include <thread>

#include <assert.h>
#include <errno.h>

#include "util.h"
#include "ctrldata.h"

//----------------------------------------------------------------------------//
CtrlData::CtrlData(iio_context * a_iio_ctx, const char * a_iio_dev) :
    m_valid(false),
    m_iio_dev(nullptr),
    m_name(a_iio_dev)
{
  m_iio_dev = ::iio_context_find_device(a_iio_ctx, a_iio_dev);
  if (m_iio_dev == nullptr) {
    std::cout << "ERROR: Find IIO device " << a_iio_dev
              << " failed. errno=" << errno
              << "-" << get_iio_strerror(errno).c_str()
              << std::endl;
    return;
  }

  m_iio_rx = ::iio_device_find_channel(m_iio_dev, "voltage0", false);
  if (m_iio_rx == nullptr) {
    std::cout << "ERROR: Find IIO channel in_voltage0 failed. errno=" << errno
              << "-" << get_iio_strerror(errno).c_str()
              << std::endl;
    return;
  }
  m_iio_rxlo = ::iio_device_find_channel(m_iio_dev, "altvoltage0", true);
  if (m_iio_rxlo == nullptr) {
    std::cout << "ERROR: Find IIO channel out_altvoltage0 failed. errno=" << errno
              << "-" << get_iio_strerror(errno).c_str()
              << std::endl;
    return;
  }

  m_iio_tx = ::iio_device_find_channel(m_iio_dev, "voltage0", true);
  if (m_iio_tx == nullptr) {
    std::cout << "ERROR: Find IIO channel out_voltage0 failed. errno=" << errno
              << "-" << get_iio_strerror(errno).c_str()
              << std::endl;
    return;
  }
  m_iio_txlo = ::iio_device_find_channel(m_iio_dev, "altvoltage1", true);
  if (m_iio_txlo == nullptr) {
    std::cout << "ERROR: Find IIO channel out_altvoltage1 failed. errno=" << errno
              << "-" << get_iio_strerror(errno).c_str()
              << std::endl;
    return;
  }

  std::cout << "* Found IIO device: " << a_iio_dev << std::endl;

  ::iio_channel_enable(m_iio_rx);
  ::iio_channel_enable(m_iio_rxlo);
  ::iio_channel_disable(m_iio_tx);
  ::iio_channel_disable(m_iio_txlo);

  bool bRet;

  bRet = set_rxlo_powerdown(ePowerUp);
  assert((bRet == true) && ":set_rxlo_powerdown failed.");

  bRet = set_txlo_powerdown(ePowerDown);
  assert((bRet == true) && ":set_txlo_powerdown failed.");

  bRet = set_loopback(eLoopbackDisabled);
  assert((bRet == true) && ":set_loopback failed.");

//  bRet = set_rxtx_filter(GSM_ftr_len, GSM_ftr);
//  assert((bRet == true) && ":set_rxtx_filter failed.");

  bRet = set_rx_gain(eHwGainModeManual, 20);
  assert((bRet == true) && ":set_rx_gain failed.");

  bRet = set_rx_bw(200000);
  assert((bRet == true) && ":set_rx_bw failed.");

  bRet = set_tx_bw(200000);
  assert((bRet == true) && ":set_tx_bw failed.");

  bRet = set_samplingfreq(2166666); //61440000, 30720000
  assert((bRet == true) && ":set_samplingfreq failed.");

  bRet = set_rx_lofreq(462000000);
  assert((bRet == true) && ":set_rx_lofreq failed.");

  bRet = set_tx_lofreq(462000000);
  assert((bRet == true) && ":set_tx_lofreq failed.");

  bRet = set_tx_gain(-10.0);
  assert((bRet == true) && ":set_tx_gain failed.");

  //bRet = set_loopback(eLoopbackDigital);
  //assert((bRet == true) && ":set_loopback failed.");

  //TODO: Does not work: initialize();
  //iio_attr -d ad9361-phy calib_mode manual
  set_calib_mode(eCalibMode_auto);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//  set_calib_mode(eCalibMode_rf_dc_offs);
//  std::this_thread::sleep_for(std::chrono::milliseconds(1500));

  //pluto_init_txrx:

  //pluto_enable_fir(0):
  //ad9361_set_trx_fir_enable(phy, false);
  //pluto_set_filter():
  //iio_device_attr_write_raw(phy, "filter_fir_config", LTE1p4_MHz_ftr, LTE1p4_MHz_ftr_len);
  //iio_device_attr_write_raw(phy, "filter_fir_config", LTE20_MHz_ftr, LTE20_MHz_ftr_len );
  //pluto_enable_fir(1):
  //ad9361_set_trx_fir_enable(phy, true);

  //pluto_set_out_gain( -80 ):
  //iio_channel_attr_write_longlong(iio_device_find_channel(phy, "voltage0", true), "hardwaregain", gain);

  //iio_channel_attr_write_longlong(iio_device_find_channel(phy, "voltage0", false), "hardwaregain", 70);
  //iio_channel_attr_write(iio_device_find_channel(phy, "voltage0", false), "gain_control_mode", "slow_attack");

  //pluto_set_in_gain if in manual mode:
  //iio_channel_attr_write(iio_device_find_channel(phy, "voltage0", false), "gain_control_mode", "manual");
  //iio_channel_attr_write_longlong(iio_device_find_channel(phy, "voltage0", false), "hardwaregain", gain);


  //pluto_set_in_sample_freq( sample_freq_hz ): //11200000
  //iio_channel_attr_write_longlong(iio_device_find_channel(phy, "voltage0", false), "sampling_frequency", sfreq);


  //pluto_set_in_bw( rf_bandwidth ): //250000
  //iio_channel_attr_write_longlong(iio_device_find_channel(phy, "voltage0", false), "rf_bandwidth", chbw);


  //pluto_set_out_bw( rf_bandwidth ): //250000
  //iio_channel_attr_write_longlong(iio_device_find_channel(phy, "voltage0", true), "rf_bandwidth", chbw);


  //pluto_set_rx_freq(freq_rxtx_hz, ref_correction_ppm):
  //iio_channel_attr_read_longlong(iio_device_find_channel(phy, "voltage0", false), "sampling_frequency", &current_sample_freq);
  //iio_channel_attr_write_longlong(iio_device_find_channel(phy, "altvoltage0", true), "frequency", current_rx_freq ); //929400000
  //iio_channel_attr_write_longlong(iio_device_find_channel(phy, "altvoltage1", true), "frequency", current_rx_freq);  //929400000


  //pluto_set_out_gain( -80 ): //-10
  //iio_channel_attr_write_longlong(iio_device_find_channel(phy, "voltage0", true), "hardwaregain", gain);


  m_valid = true;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
CtrlData::~CtrlData()
{
  if ((m_iio_rx != nullptr) && (::iio_channel_is_enabled(m_iio_rx) == true)) {
    ::iio_channel_disable(m_iio_rx);
  }
  if ((m_iio_rxlo != nullptr) && (::iio_channel_is_enabled(m_iio_rxlo) == true)) {
    ::iio_channel_disable(m_iio_rxlo);
  }
  if ((m_iio_tx != nullptr) && (::iio_channel_is_enabled(m_iio_tx) == true)) {
    ::iio_channel_disable(m_iio_tx);
  }
  if ((m_iio_txlo != nullptr) && (::iio_channel_is_enabled(m_iio_txlo) == true)) {
    ::iio_channel_disable(m_iio_txlo);
  }

  std::cout << "Destructor of: " << m_name << " CtrlData done."
            << std::endl;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
bool CtrlData::initialize()
{
  //iio_attr -d ad9361-phy calib_mode manual
  set_calib_mode(eCalibMode_manual);
  std::this_thread::sleep_for(std::chrono::milliseconds(1500));
  //iio_attr -c ad9361-phy -o altvoltage1 powerdown 1
  set_rxlo_powerdown(ePowerDown);
  //iio_attr -c ad9361-phy -o altvoltage0 powerdown 1
  set_txlo_powerdown(ePowerDown);
  //iio_attr -D ad9361-phy initialize 1
  std::this_thread::sleep_for(std::chrono::milliseconds(1500));
  long long val(1);
  int ret = ::iio_device_debug_attr_write_longlong(m_iio_dev, "initialize", val);
  if (ret != 0) {
    std::cout << "ERROR: iio_device_debug_attr_write_longlong. ret=" << ret << "-" << get_iio_strerror(ret).c_str()
              << std::endl;
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(1500));

  return (ret == 0);
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
bool CtrlData::set_calib_mode(calib_mode_t a_mode)
{
  int ret = -1;

  if (a_mode == eCalibMode_auto) { ret = ::iio_device_attr_write(m_iio_dev, "calib_mode", "auto"); }
  else if (a_mode == eCalibMode_manual) { ret = ::iio_device_attr_write(m_iio_dev, "calib_mode", "manual\0"); }
  else if (a_mode == eCalibMode_manual_tx_quad) { ret = ::iio_device_attr_write(m_iio_dev, "calib_mode", "manual_tx_quad"); }
  else if (a_mode == eCalibMode_tx_quad) { ret = ::iio_device_attr_write(m_iio_dev, "calib_mode", "tx_quad"); }
  else if (a_mode == eCalibMode_rf_dc_offs) { ret = ::iio_device_attr_write(m_iio_dev, "calib_mode", "rf_dc_offs"); }
  else if (a_mode == eCalibMode_rssi_gain_step) { ret = ::iio_device_attr_write(m_iio_dev, "calib_mode", "rssi_gain_step"); }
  else return false;

  if (ret < 0) {
    std::cout << "ERROR: set_calib_mode. ret=" << ret << "-" << get_iio_strerror(ret).c_str()
              << std::endl;
  }
  return (ret == 0);
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
bool CtrlData::get_rssi(double &a_rssi)
{
  double val;
  int ret = ::iio_channel_attr_read_double(m_iio_rx, "rssi", &val);
  a_rssi = double(val);

  if (ret != 0) {
    std::cout << "ERROR: get_rssi. ret=" << ret << "-" << get_iio_strerror(ret).c_str()
              << std::endl;
  }
  return (ret == 0);
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
bool CtrlData::get_rx_gain(int32_t &a_rx_gain)
{
  long long val;
  int ret = ::iio_channel_attr_read_longlong(m_iio_rx, "hardwaregain", &val);
  a_rx_gain = int32_t(val);

  if (ret != 0) {
    std::cout << "ERROR: get_rx_gain. ret=" << ret << "-" << get_iio_strerror(ret).c_str()
              << std::endl;
  }
  return (ret == 0);
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
bool CtrlData::get_tx_gain(double &a_tx_gain)
{
  double val;
  int ret = ::iio_channel_attr_read_double(m_iio_tx, "hardwaregain", &val);
  a_tx_gain = double(val);

  if (ret != 0) {
    std::cout << "ERROR: get_tx_gain. ret=" << ret << "-" << get_iio_strerror(ret).c_str()
              << std::endl;
  }
  return val;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
bool CtrlData::set_rx_gain(hwgain_mode_t a_mode, int32_t a_gain)
{
  int ret = -1;

  if (a_mode == eHwGainModeManual) {
    // [-3 1 71]
    if ((a_gain < -3) || (a_gain > 71))
      return false;

    ::iio_channel_attr_write(m_iio_rx, "gain_control_mode", "manual");
    ret = ::iio_channel_attr_write_double(m_iio_rx, "hardwaregain", a_gain);

    if (ret != 0) {
      std::cout << "ERROR: set_rx_gain. ret=" << ret << "-" << get_iio_strerror(ret).c_str()
                << std::endl;
    }
    return (ret == 0);
  }

  if (a_mode == eHwGainModeFast)  {
    ret = ::iio_channel_attr_write(m_iio_rx, "gain_control_mode", "fast_attack");
  }
  else if (a_mode == eHwGainModeSlow) {
    ret = ::iio_channel_attr_write(m_iio_rx, "gain_control_mode", "slow_attack");
  }
  else if (a_mode == eHwGainModeHybrid) {
    ret = ::iio_channel_attr_write(m_iio_rx, "gain_control_mode", "hybrid");
  }
  else return false;

  if (ret < 0) {
    std::cout << "ERROR: set_rx_gain. ret=" << ret << "-" << get_iio_strerror(ret).c_str()
              << std::endl;
  }
  return (ret > 0);
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
bool CtrlData::set_tx_gain(double a_gain)
{
  // [-89.75 0.25 0.00]
  if ((a_gain < double(-89.75)) || (a_gain > double(0.0)))
    return false;

  int ret = ::iio_channel_attr_write_double(m_iio_tx, "hardwaregain", a_gain);
  if (ret != 0) {
    std::cout << "ERROR: set_tx_gain. ret=" << ret << "-" << get_iio_strerror(ret).c_str()
              << std::endl;
  }
  return (ret == 0);
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
bool CtrlData::set_rx_bw(uint32_t a_bw)
{
  //[200000 1 56000000]
  if ((a_bw < 200000) || (a_bw > 56000000))
    return false;

  long long val = a_bw;
  int ret = ::iio_channel_attr_write_longlong(m_iio_rx, "rf_bandwidth", val);
  if (ret != 0) {
    std::cout << "ERROR: set_rx_bw. ret=" << ret << "-" << get_iio_strerror(ret).c_str()
              << std::endl;
  }
  return (ret == 0);
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
bool CtrlData::set_tx_bw(uint32_t a_bw)
{
  //[200000 1 40000000]
  if ((a_bw < 200000) || (a_bw > 40000000))
    return false;

  long long val = a_bw;
  int ret = ::iio_channel_attr_write_longlong(m_iio_tx, "rf_bandwidth", val);
  if (ret != 0) {
    std::cout << "ERROR: set_tx_bw. ret=" << ret << "-" << get_iio_strerror(ret).c_str()
              << std::endl;
  }
  return (ret == 0);
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
bool CtrlData::set_samplingfreq(uint32_t a_sfreq)
{
  //[2083333 1 61440000]
  if ((a_sfreq < 2083333) || (a_sfreq > 61440000))
    return false;

  long long val = a_sfreq;
  int ret = ::iio_channel_attr_write_longlong(m_iio_rx, "sampling_frequency", val);
  if (ret != 0) {
    std::cout << "ERROR: set_samplingfreq. ret=" << ret << "-" << get_iio_strerror(ret).c_str()
              << std::endl;
  }
  return (ret == 0);
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
bool CtrlData::set_rx_lofreq(uint64_t a_lofreq)
{
  //[70000000 1 6000000000]
  if ((a_lofreq < 70000000LL) || (a_lofreq > 6000000000LL))
    return false;

  long long val = a_lofreq;
  int ret = ::iio_channel_attr_write_longlong(m_iio_rxlo, "frequency", val);
  if (ret != 0) {
    std::cout << "ERROR: set_rx_lofreq. ret=" << ret << "-" << get_iio_strerror(ret).c_str()
              << std::endl;
  }
  return (ret == 0);
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
bool CtrlData::set_tx_lofreq(uint64_t a_lofreq)
{
  //[46875001 1 6000000000]
  if ((a_lofreq < 46875001LL) || (a_lofreq > 6000000000LL))
    return false;

  long long val = a_lofreq;
  int ret = ::iio_channel_attr_write_longlong(m_iio_txlo, "frequency", val);
  if (ret != 0) {
    std::cout << "ERROR: set_tx_lofreq. ret=" << ret << "-" << get_iio_strerror(ret).c_str()
              << std::endl;
  }
  return (ret == 0);
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
bool CtrlData::set_rxlo_powerdown(powerdown_t a_mode)
{
  long long val = long(a_mode);
  int ret = ::iio_channel_attr_write_longlong(m_iio_rxlo, "powerdown", val);
  if (ret != 0) {
    std::cout << "ERROR: set_rxlo_powerdown. ret=" << ret << "-" << get_iio_strerror(ret).c_str()
              << std::endl;
  }
  return (ret == 0);
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
bool CtrlData::set_txlo_powerdown(powerdown_t a_mode)
{
  long long val = long(a_mode);
  int ret = ::iio_channel_attr_write_longlong(m_iio_txlo, "powerdown", val);
  if (ret != 0) {
    std::cout << "ERROR: set_txlo_powerdown. ret=" << ret << "-" << get_iio_strerror(ret).c_str()
              << std::endl;
  }
  return (ret == 0);
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
bool CtrlData::set_loopback(looback_t a_mode)
{
  long long val = long(a_mode);
  int ret = ::iio_device_debug_attr_write_longlong(m_iio_dev, "loopback", val);
  if (ret != 0) {
    std::cout << "ERROR: set_loopback. ret=" << ret << "-" << get_iio_strerror(ret).c_str()
              << std::endl;
  }
  return (ret == 0);
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
/*
bool CtrlData::set_rxtx_filter(unsigned int a_len, const unsigned char* a_data)
{
  int ret = ::ad9361_set_trx_fir_enable(m_iio_dev, 0);
  if (ret != 0) {
    std::cout << "ERROR: ad9361_set_trx_fir_enable. ret=" << ret << "-" << get_iio_strerror(ret).c_str()
              << std::endl;
    return false;
  }

  if ((a_len > 0) && (a_data != nullptr)) {
    ret = ::iio_device_attr_write_raw(m_iio_dev, "filter_fir_config", a_data, a_len);
    if (ret != a_len) {
      std::cout << "ERROR: iio_device_attr_write_raw. ret=" << ret << "-" << get_iio_strerror(ret).c_str()
                << std::endl;
      return false;
    }

    ret = ::ad9361_set_trx_fir_enable(m_iio_dev, 1);
    if (ret != 0) {
      std::cout << "ERROR: ad9361_set_trx_fir_enable. ret=" << ret << "-" << get_iio_strerror(ret).c_str()
                << std::endl;
    }
  }

  return (ret == 0);
}
*/
//----------------------------------------------------------------------------//
