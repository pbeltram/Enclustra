#!/usr/bin/env python3

"""Description of fmcomms3_dac_test.py"""
"""Test DAC in DMA in loopback."""

import site
import importlib.util
import sys
#import iio
import numpy as np
import matplotlib.pyplot as plt
#from mpldatacursor import datacursor

# User configurable
TXLO = 500000000
TXBW = 5000000
TXFS = 3000320
RXLO = TXLO
RXBW = TXBW
RXFS = TXFS

#-------------------------------------------------------------------------------
def myFormatter(**kwarg):
    label = 'x:{x:.0f}\ny:{y:.0f}'.format(**kwarg)
    return label
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
def create_ramp():
    N = 2**10 # Dont exceed 2**11
    triag = np.int16(range(int(N)))
    triag = np.concatenate((triag, triag[::-1]), axis=0)
    triag = triag*16; # Shift to prevent interface core cast scale reduction
    iq = np.empty((triag.size*2,), dtype=np.int16)
    iq[0::2] = triag
    iq[1::2] = triag
    iq = np.int16(iq)
    samples_per_channel = iq.size//2

    return iq, samples_per_channel, triag, triag
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
def create_sin(a_fc, a_Fs):
    num_samples = a_Fs//a_fc
    repeat = 4
    nbits = 10
    x = np.linspace(0, repeat*(2*np.pi), num=repeat*(num_samples), endpoint=False)
    sin_i = np.sin(x+np.pi)*(2**(nbits-1)-1)
    cos_q = np.cos(x+np.pi)*(2**(nbits-1)-1)
    sin_i = sin_i*16; # Shift to prevent interface core cast scale reduction
    cos_q = cos_q*16; # Shift to prevent interface core cast scale reduction
    iq = np.empty((sin_i.size+cos_q.size,), dtype=np.int16)
    iq[0::2] = sin_i
    iq[1::2] = cos_q
    iq = np.int16(iq)

    return iq, int(num_samples*repeat), sin_i, cos_q
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
def plot_fft(a_name, a_Fs, a_sig):
    N = len(a_sig)
    freq = np.arange(N)*a_Fs/N -a_Fs/2

    X = np.fft.fft(a_sig, n=N)
    #print(type(X[0]))
    #X.tofile('./sig_X_fft.dat')
    X = np.fft.fftshift(X)
    #print(type(X[0]))
    #X.tofile('./sig_X_fftshift.dat')
    X = 1/N*X
    #print(type(X[0]))
    #X.tofile('./sig_X.dat')
    yf = np.abs(X)
    #print(type(yf[0]))
    #yf.tofile('./sig_yf_abs.dat')
    if (min(yf) != 0):
        yf = 20*np.log10(yf)
    #print(type(yf[0]))
    #yf.tofile('./sig_yf_20log10.dat')
    plt.figure(a_name)
    yflines = plt.plot(freq, yf)
    plt.grid(True)
    plt.suptitle('FFT (%u points)' % N)
#    datacursor(yflines, formatter=myFormatter)

    # Filter out phase noise
    X2=X
    threshold = max(np.abs(X))/1000
    #X2[np.abs(X)<threshold] = 0
    #X2 = np.where(X < threshold, 0, X)  # set value '0' where condition is met
    ph = np.arctan2(np.imag(X2), np.real(X2))*180/np.pi
    #print(type(ph[0]))
    #ph.tofile('./sig_ph.dat')
    plt.figure("Phase spectrum")
    phlines = plt.plot(freq, ph)
    plt.grid(True)
    plt.suptitle('FFT (%u points)' % N)
#    datacursor(phlines, formatter=myFormatter)
    return
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
def plot_sig(a_name, a_txsig, a_rxsig):
    plt.figure(a_name)
    ylines = plt.plot(a_rxsig)
    plt.plot(a_txsig)
    plt.xlabel("Samples")
    plt.ylabel("Amplitude")
    plt.grid(True)
#    datacursor(ylines, formatter=myFormatter)

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

    ctrl.find_channel('altvoltage0', True).attrs["powerdown"].value = str(int(0))
    ctrl.find_channel('altvoltage1', True).attrs["powerdown"].value = str(int(0))

    # Configure transceiver settings
    ctrl.find_channel('altvoltage0', True).attrs["frequency"].value = str(int(RXLO))
    ctrl.find_channel('altvoltage1', True).attrs["frequency"].value = str(int(TXLO))
    ctrl.find_channel('voltage0', False).attrs["rf_bandwidth"].value = str(int(RXBW))
    ctrl.find_channel('voltage0', True).attrs["rf_bandwidth"].value = str(int(TXBW))
    ctrl.find_channel('voltage0', False).attrs["sampling_frequency"].value = str(int(RXFS))
    ctrl.find_channel('voltage0', True).attrs["sampling_frequency"].value = str(int(TXFS))
    #NOTE: Valid only if "gain_control_mode" in "manual" mode: ctrl.find_channel('voltage0', False).attrs['hardwaregain'].value = '30'
    ctrl.find_channel('voltage0', False).attrs['gain_control_mode'].value = 'slow_attack'
    #ctrl.find_channel('voltage0', False).attrs['gain_control_mode'].value = 'manual'
    #ctrl.find_channel('voltage0', False).attrs['hardwaregain'].value = '30' # RX HW gain
    ctrl.find_channel('voltage0', True).attrs['hardwaregain'].value = '-10' # TX Attenuation

    # Enable all IQ channels
    rxadc.find_channel("voltage0", False).enabled = True
    rxadc.find_channel("voltage1", False).enabled = True

    # Force DAC to use DMA not DDS
    txdac.find_channel("voltage0",True).enabled = True
    txdac.find_channel("voltage1", True).enabled = True
    txdac.find_channel('TX1_I_F1', True).attrs['raw'].value = str(0)
    txdac.find_channel('TX1_Q_F1', True).attrs['raw'].value = str(0)
    txdac.find_channel('TX1_I_F2', True).attrs['raw'].value = str(0)
    txdac.find_channel('TX1_Q_F2', True).attrs['raw'].value = str(0)

    ctrl.debug_attrs['loopback'].value = '0' #  0-Disabled, 1-Dig_TX->Dig_RX, 2-Rf_TX-Rf_RX

    #iq, samples_per_channel, dac_i, dac_q = create_ramp()
    iq, samples_per_channel, dac_i, dac_q = create_sin(a_fc=11720, a_Fs=RXFS)

    # Create cyclic buffer for TX data
    txbuf = libiio.Buffer(txdac, samples_per_channel, True)
    # Send data to buffer
    txbuf.write(bytearray(iq))
    txbuf.push()

    # Create buffer for 65536 RX data
    rxbuf = libiio.Buffer(rxadc, 2**16, False)
    for k in range(3):
      rxbuf.refill()
      data = rxbuf.read()
      x = np.frombuffer(data,dtype=np.int16)
      sig = x[::2] + 1j * x[1::2]

    #print(type(sig[0]))
    #sig.tofile('./sig_iq.cf128')

    plot_sig(a_name="FMCOMMS2 DAC", a_txsig=dac_i/16, a_rxsig=np.real(sig))

    plot_fft(a_name="Rx signal CFFT", a_Fs=RXFS, a_sig=sig)

    plt.draw()
    plt.show()

    return
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
if __name__ == "__main__":
    assert sys.version_info >= (3, 5)

    # Workaround same module name: Analog IIO and gr-iio
    spec = importlib.util.spec_from_file_location("module.name", site.USER_SITE + "/iio.py")
    libiio = importlib.util.module_from_spec(spec)
    sys.modules["module.name"] = libiio
    spec.loader.exec_module(libiio)

    main(sys.argv[1:])
#-------------------------------------------------------------------------------
