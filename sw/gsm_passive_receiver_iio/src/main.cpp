/*
 * main.cpp
 *
 *  Created on: Oct 7, 2023
 *      Author: primozb
 */

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <string>
#include <memory>
#include <atomic>
#include <chrono>
#include <thread>
#include <mutex>
#include <list>
#include <csignal>

#include <assert.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>

#include <iio.h>

#include "gr_complex.h"

#include <arfcn.hpp>

#include "main.hpp"
#include "ctrldata.h"
#include "signaldata.h"

#include "gsm_constants.h"
#include "receiver_worker.h"

//----------------------------------------------------------------------------//
static uint32_t g_arfcn_no = 1;
static bool g_arfcn_ul = false;
static uint32_t g_timeout = 0;
static uint32_t g_nof_a = 80*1024;
static std::string g_iioinp("ip:xu8");
static uint32_t g_osr = 8;
static double g_mva_threshold = 0.3;
static bool g_write = false;
static bool g_write_burst = false;
static int32_t g_gain = 27;
static float g_freq_offs = -1950.0;
static std::string g_udp_dest("127.0.0.1");

std::mutex g_data_mutex;
static std::atomic<bool> g_main_stop{false};
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
static void usage(const char *argv0)
{
  std::cout << "Usage: " << argv0 << " [options]" << std::endl;
  std::cout <<
      "-n, --nofa n<k|M> Number of ch data to read in unit, kilo or Mega samples (default=80k)."
      << std::endl;
  std::cout <<
      "-i, --input       IIO input name (default=\"ip:xu8\")."
      << std::endl;
  std::cout <<
      "-a, --arfcn       ARFCN number (default=1)."
      << std::endl;
  std::cout <<
      "-u, --uplink      ARFCN uplink (deafult=false)."
      << std::endl;
  std::cout <<
      "-t, --timeout     IIO time out in msec (default=0msec)."
      << std::endl;
  std::cout <<
      "-w, --write       Write (overwrite) samples to \"./iq_16bit.dat\" file (default=false)."
      << std::endl;
  std::cout <<
      "-s, --save        Write found bursts to \"./burst_NNN.cf64\" file (default=false)."
      << std::endl;
  std::cout <<
      "-m, --mvalevel    MVA threshold level value (default=0.025)."
      << std::endl;
  std::cout <<
      "-g, --gain        Gain value (default=27)."
      << std::endl;
  std::cout <<
      "-f, --freqoff     Frequency offset (default=-1950.0)."
      << std::endl;
  std::cout <<
      "-d, --udpdest     IP address destination of UDP host default(\"127.0.0.1\")."
      << std::endl;
  std::cout <<
      "-h, --help        Show this help screen."
      << std::endl;
}

static struct option opts[] = {
  /* These options set a flag. */
  { "nofa",      required_argument, nullptr,    'n' },
  { "input",     required_argument, nullptr,    'i' },
  { "arfcn",     required_argument, nullptr,    'a' },
  { "uplink",    no_argument,       nullptr,    'u' },
  { "timeout",   required_argument, nullptr,    't' },
  { "write",     no_argument,       nullptr,    'w' },
  { "save",      no_argument,       nullptr,    's' },
  { "mvalevel",  required_argument, nullptr,    'm' },
  { "gain",      required_argument, nullptr,    'g' },
  { "freqoff",   required_argument, nullptr,    'f' },
  { "udpdest",   required_argument, nullptr,    'd' },
  { "help",      no_argument,       nullptr,    'h' },
  { nullptr,     0,                 nullptr,     0  }
};
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
static int parse_int(char *sz_num)
{
  size_t sz_len = ::strlen(optarg);
  assert((sz_len > 0) && ":Parse sz_num length is 0.");

  int magn_val = 1;
  char *magn_pch = sz_num+sz_len-1;

  if (::isdigit(*magn_pch) == 0) {

    if ('k' == *magn_pch) magn_val = 1024;
    else if ('M' == *magn_pch) magn_val = 1024*1024;
    else {
      assert((false) && ":Parse wrong parameter magnifier.");
    }

    *magn_pch = '\0';
  }
  int ret_int = atoi(sz_num) * magn_val;

  return ret_int;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
static double parse_double(char *sz_num)
{
  size_t sz_len = ::strlen(optarg);
  assert((sz_len > 0) && ":Parse sz_num length is 0.");

  double ret_double = atof(sz_num);

  return ret_double;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
static bool parse(int argc, char *argv[])
{
  // Parse command line arguments.
  int c;
  int option_index = 0;
  while ((c = ::getopt_long(argc, argv, "n:i:s:a:ut:wsm:g:f:d:h", opts, &option_index)) != -1) {
    switch (c) {
      case 'h':
        usage(argv[0]);
        return false;

      case 'n':
        g_nof_a = parse_int(optarg);
        break;

      case 'i':
        g_iioinp = std::string(optarg);
        break;

      case 'a':
        g_arfcn_no = parse_int(optarg);
        break;

      case 'u':
        g_arfcn_ul = true;
        break;

      case 't':
        g_timeout = parse_int(optarg);
        break;

      case 'w':
        g_write = true;
        break;

      case 's':
        g_write_burst = true;
        break;

      case 'm':
        g_mva_threshold = parse_double(optarg);
        break;

      case 'g':
        g_gain = parse_int(optarg);
        break;

      case 'f':
        g_freq_offs = parse_double(optarg);
        break;

      case 'd':
        g_udp_dest = std::string(optarg);
        break;

      case '?':
        // getopt_long already printed an error message.
        std::cout << "Run " << argv[0] << " -h for help." << std::endl;
        return false;

      default:
        std::cout << "Invalid option -" << c << "." << std::endl;
        std::cout << "Run " << argv[0] << " -h for help." << std::endl;
        return false;
    }
  }

  if (optind < argc) {
    std::cout << " Parsing got non-option ARGV-elements: ";
    while (optind < argc) {
      std::cout << argv[optind++] << " ";
    }
    std::cout << std::endl;
    return false;
  }

  return true;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
void handle_sig(int sig)
{
//  std::lock_guard<std::mutex> lock_loop(g_data_mutex);

  g_main_stop = true;
  std::cout << "Sig handler: " << sig << std::endl;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
int main(int argc, char **argv) {

  if (parse(argc, argv) == false) {
    return -1;
  }

  std::cout << "INFO: Main started." << std::endl;

  iio_context * iio_ctx = iio_create_context_from_uri(g_iioinp.c_str());
  if (iio_ctx == nullptr) {
    std::cout << "ERROR: Create IIO context failed. errno=" << errno
        << "-" << ::strerror(errno)
        << std::endl;
    return errno;
  }

  int res = ::iio_context_set_timeout(iio_ctx, g_timeout);
  assert((res == 0) && ":IIO iio_context_set_timeout failed.");

  float fc = (g_arfcn_ul == true) ? arfcn2uplink(g_arfcn_no):arfcn2downlink(g_arfcn_no);

  CtrlDataPtr_type ctrl = std::make_shared<CtrlData>(iio_ctx, "ad9361-phy");
  assert((ctrl->is_valid() == true) && ":CtrlData create failed.");

  bool bRet;

  bRet = ctrl->set_rx_bw(250000);
  assert((bRet == true) && ":CtrlData set_rx_bw failed.");

  uint32_t samp_rate_in = std::lroundf(GSM_SYMBOL_RATE * g_osr); // 2.166667

  bRet = ctrl->set_samplingfreq(samp_rate_in);
  assert((bRet == true) && ":CtrlData set_samplingfreq failed.");

  bRet = ctrl->set_rx_lofreq(uint64_t(fc));
  assert((bRet == true) && ":CtrlData set_rx_lofreq failed.");

  bRet = ctrl->set_rx_gain(CtrlData::eHwGainModeManual, g_gain);
  assert((bRet == true) && ":CtrlData set_rx_gain failed.");

  SignalDataPtr_type sig_ch = std::make_shared<SignalData>(iio_ctx, "cf-ad9361-lpc", g_nof_a);
  assert((sig_ch->is_valid() == true) && ":SignalData create failed.");

  std::signal(SIGINT, handle_sig);

  std::cout
      << "ARFCN=" << std::dec << std::setprecision(3) << g_arfcn_no
      << " Gain=" << std::fixed << std::setprecision(3) << g_gain
      << " FC=" << std::fixed << std::setprecision(3) << fc
      << " FS=" << std::fixed << std::setprecision(1) << samp_rate_in
      << " FOffs=" << std::fixed << std::setprecision(1) << g_freq_offs
      << " MVA=" << std::fixed << std::setprecision(3) << g_mva_threshold
      << " OSR=" << std::dec << g_osr
      << std::endl;

  receiver_worker* preceiver = new receiver_worker(g_main_stop, sig_ch,
      g_mva_threshold, g_freq_offs, g_arfcn_no, g_arfcn_ul, g_write, g_write_burst, g_udp_dest);

  preceiver->run();

  delete preceiver;

  assert((sig_ch.use_count() == 1) && ":SignalData bad use_count.");
  sig_ch.reset(); // Delete shared pointer instance.

  std::this_thread::sleep_for(std::chrono::seconds(1));

  if (iio_ctx) {
    ::iio_context_destroy(iio_ctx);
  }

  std::cout << "INFO: Main done." << std::endl;

  return 0;
}
//----------------------------------------------------------------------------//
