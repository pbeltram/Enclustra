#!/usr/bin/env python3

"""Description of fmcomms3_rx_test.py"""
"""Test RX."""

import site
import importlib.util
import sys
#import iio
import numpy as np
import matplotlib.pyplot as plt
#from mpldatacursor import datacursor

# User configurable
RXLO = 1842.6e6
RXBW = 5000000
RXFS = 61440000

#-------------------------------------------------------------------------------
def myFormatter(**kwarg):
    label = 'x:{x:.0f}\ny:{y:.0f}'.format(**kwarg)
    return label
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
def plot_sig(a_name, a_rxsig):
    plt.figure(a_name)
    ylines = plt.plot(a_rxsig)
    plt.xlabel("Samples")
    plt.ylabel("Amplitude")
#    datacursor(ylines, formatter=myFormatter)
    plt.grid(True)

    return
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
def plot_fft(a_name, a_Fs, a_sig, a_plot_phase = False):
    N = len(a_sig)
    freq = np.arange(N)*a_Fs/N -a_Fs/2

    X = 1/N*np.fft.fftshift(np.fft.fft(a_sig, n=N))
    yf = 20*np.log10(np.abs(X))
    plt.figure(a_name)
    yflines = plt.plot(freq, yf)
    plt.grid(True)
    plt.suptitle('FFT (%u points)' % N)
#    datacursor(yflines, formatter=myFormatter)

    if (a_plot_phase == True):
        # Filter out phase noise
        X2=X
        threshold = max(np.abs(X))/1000
        #X2[np.abs(X)<threshold] = 0
        #X2 = np.where(X < threshold, 0, X)  # set value '0' where condition is met
        ph = np.arctan2(np.imag(X2), np.real(X2))*180/np.pi
        plt.figure("Phase spectrum")
        phlines = plt.plot(freq, ph)
        plt.grid(True)
        plt.suptitle('FFT (%u points)' % N)
#        datacursor(phlines, formatter=myFormatter)

    return
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
def main(argv):
    try:
        ctx = libiio.Context('ip:xu8')
    except:
        print("No device found")
        sys.exit(0)

    ctrl = ctx.find_device("ad9361-phy")
    txdac = ctx.find_device("cf-ad9361-dds-core-lpc")
    rxadc = ctx.find_device("cf-ad9361-lpc")

    ctrl.debug_attrs['loopback'].value = '0' #  0-Disabled, 1-Dig_TX->Dig_RX, 2-Rf_TX-Rf_RX
    ctrl.find_channel('altvoltage0', True).attrs["powerdown"].value = str(int(0))
    ctrl.find_channel('altvoltage1', True).attrs["powerdown"].value = str(int(0))

    # Configure transceiver settings
    ctrl.find_channel('altvoltage0', True).attrs["frequency"].value = str(int(RXLO))
    ctrl.find_channel('voltage0', False).attrs["sampling_frequency"].value = str(int(RXFS))
    ctrl.find_channel('voltage0', False).attrs["rf_bandwidth"].value = str(int(RXBW))
    ctrl.find_channel('voltage0', False).attrs['gain_control_mode'].value = 'slow_attack'

    # Enable RX IQ channels
    rxadc.find_channel("voltage0", False).enabled = True
    rxadc.find_channel("voltage1", False).enabled = True

    # Disable TX IQ channels
    txdac.find_channel("voltage0", True).enabled = False
    txdac.find_channel("voltage1", True).enabled = False
    ctrl.find_channel('altvoltage1', True).attrs["powerdown"].value = str(int(1))

    # Create buffer for 65536 RX data
    rxbuf = libiio.Buffer(rxadc, 2**16, False)
    for k in range(3):
        rxbuf.refill()
        data = rxbuf.read()
        x = np.frombuffer(data,dtype=np.int16)
        sig = x[::2] + 1j * x[1::2]

    plt.figure("FMCOMMS3 RX")
    ylines = plt.plot(np.real(sig))
    #plt.plot(np.imag(sig))
    plt.xlabel("Samples")
    plt.ylabel("Amplitude")
#    datacursor(ylines, formatter=myFormatter)
    plt.grid(True)

    plot_sig(a_name="FMCOMMS3 RX", a_rxsig=np.real(sig))

    plot_fft(a_name="Rx signal CFFT", a_Fs=RXFS, a_sig=sig, a_plot_phase=False)

    plt.draw()
    plt.show()

    return
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
if __name__ == "__main__":
    assert sys.version_info >= (3, 5)

    # Workaround same iio module name: Analog IIO and gr-iio
    print(site.getsitepackages())
    spec = importlib.util.spec_from_file_location("module.name", site.USER_SITE + "/iio.py")
    libiio = importlib.util.module_from_spec(spec)
    sys.modules["module.name"] = libiio
    spec.loader.exec_module(libiio)

    main(sys.argv[1:])
#-------------------------------------------------------------------------------
