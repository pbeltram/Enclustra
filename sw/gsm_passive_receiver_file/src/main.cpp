// See LICENSE ane README.md files.

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <algorithm>
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

#include "gr_complex.h"
#include "receiver_worker.h"
#include "main.hpp"

//----------------------------------------------------------------------------//
static uint32_t g_nof_a = 10*1024;
static std::string g_finp("./input_raw.cf64");
static uint32_t g_decimate = 1;
static uint32_t g_sampling_ratio = 1;
static double g_mva_threshold = 0.025;
static bool g_write = false;

static std::atomic<bool> g_main_stop{false};
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
static void usage(const char *argv0)
{
  std::cout << "Usage: " << argv0 << " [options]" << std::endl;

  std::cout <<
      "-n, --nofa n<k|M> Number of ch data to read in unit, kilo or Mega samples (default=10k).\n"
      << std::endl;
  std::cout <<
      "-i, --input       Input file name default(\"./input_raw.cf64\")."
      << std::endl;
  std::cout <<
      "-a, --arfcn       ARFCN number (default=1)."
      << std::endl;
  std::cout <<
      "-u, --uplink      ARFCN uplink (deafult=false)."
      << std::endl;
  std::cout <<
      "-d, --decimate    Decimation rate (default=8)."
      << std::endl;
  std::cout <<
      "-r, --ratio       Sampling ratio (r * 1.083333) (default=1)."
      << std::endl;
  std::cout <<
      "-w, --write       Write found bursts to \"./burst_NNN.cf64\" file (default=false)."
      << std::endl;
  std::cout <<
      "-m, --mvalevel    MVA threshold level value (default=0.025)."
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
  { "decimate",  required_argument, nullptr,    'd' },
  { "ratio",     required_argument, nullptr,    'r' },
  { "write",     no_argument,       nullptr,    'w' },
  { "mvalevel",  required_argument, nullptr,    'm' },
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
  while ((c = ::getopt_long(argc, argv, "n:i:s:a:ud:r:wm:h", opts, &option_index)) != -1) {
    switch (c) {
      case 'h':
        usage(argv[0]);
        return false;

      case 'n':
        g_nof_a = parse_int(optarg);
        break;

      case 'i':
        g_finp = std::string(optarg);
        break;

      case 'd':
        g_decimate = parse_int(optarg);
        break;

      case 'r':
        g_sampling_ratio = parse_int(optarg);
        break;

      case 'w':
        g_write = true;
        break;

      case 'm':
        g_mva_threshold = parse_double(optarg);
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
  std::cout << "Sig handler: " << sig << std::endl;
  g_main_stop = true;
}
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
int main(int argc, char **argv) {

  if (parse(argc, argv) == false) {
    return -1;
  }

  std::cout << "INFO: Main started." << std::endl;

  std::signal(SIGINT, handle_sig);

  receiver_worker* preceiver = new receiver_worker(g_main_stop,
      g_nof_a, g_finp, g_decimate, g_sampling_ratio, g_mva_threshold,
      g_write);

  preceiver->run();

  delete preceiver;

  std::cout << "INFO: Main done." << std::endl;

  return 0;
}
//----------------------------------------------------------------------------//
