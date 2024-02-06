/*
 * ctrldata.h
 *
 *  Created on: Oct 7, 2023
 *      Author: primozb
 */

#ifndef _CTRL_DATA_H_
#define _CTRL_DATA_H_

#include <string>

#include <cstddef>
#include <complex>
#include <memory>

#include <iio.h>

//----------------------------------------------------------------------------//
class CtrlData
{
public:
  typedef enum hwgain_mode_ids {
    eHwGainModeManual = 0,
    eHwGainModeFast,
    eHwGainModeSlow,
    eHwGainModeHybrid,
  } hwgain_mode_t;

  typedef enum looback_ids {
    eLoopbackDisabled = 0,
    eLoopbackDigital,
    eLoopbackRF,
  } looback_t;

  typedef enum powerdown_ids {
    ePowerUp = 0,
    ePowerDown = 1,
  } powerdown_t;

  typedef enum calib_mode_ids {
    eCalibMode_auto = 0,
    eCalibMode_manual,
    eCalibMode_manual_tx_quad,
    eCalibMode_tx_quad,
    eCalibMode_rf_dc_offs,
    eCalibMode_rssi_gain_step,
  } calib_mode_t;

  CtrlData(iio_context * a_iio_ctx = NULL, const char * a_iio_dev = NULL);

  ~CtrlData();

  bool is_valid() { return m_valid; }
  const std::string& get_name() { return m_name; };

  bool initialize();

  bool get_rssi(double &a_rssi);
  bool get_rx_gain(int32_t &a_rx_gain);
  bool get_tx_gain(double &a_tx_gain);
  bool set_rx_gain(hwgain_mode_t a_mode, int32_t a_gain);
  bool set_tx_gain(double a_gain);
  bool set_rx_bw(uint32_t a_bw);
  bool set_tx_bw(uint32_t a_bw);
  bool set_samplingfreq(uint32_t a_sfreq);
  bool set_rx_lofreq(uint64_t a_lofreq);
  bool set_tx_lofreq(uint64_t a_lofreq);
  bool set_rxlo_powerdown(powerdown_t a_mode);
  bool set_txlo_powerdown(powerdown_t a_mode);
  bool set_loopback(looback_t a_mode);
  bool set_calib_mode(calib_mode_t a_mode);

private:
  bool m_valid;

  iio_device *m_iio_dev;

  iio_channel *m_iio_rx;
  iio_channel *m_iio_rxlo;

  iio_channel *m_iio_tx;
  iio_channel *m_iio_txlo;

  std::string m_name;
};
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
typedef std::shared_ptr<CtrlData> CtrlDataPtr_type;
//----------------------------------------------------------------------------//

#endif //_CTRL_DATA_H_
