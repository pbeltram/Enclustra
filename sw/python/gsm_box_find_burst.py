#!/usr/bin/env python3

import sys
import numpy as np
import matplotlib
matplotlib.use('TKAgg')
#matplotlib.use('Qt5Agg')
import matplotlib.pyplot as plt
import ctypes
import math

#-------------------------------------------------------------------------------
gsmboxlib = ctypes.cdll.LoadLibrary("./libgsmbox.so")

C_CHAN_IMP_RESP_LENGTH = ctypes.c_uint.in_dll(gsmboxlib, "c_chan_imp_resp_length").value
C_PROCESSED_CHUNK = ctypes.c_float.in_dll(gsmboxlib, "c_processed_chunk").value
C_BURST_SIZE = ctypes.c_uint.in_dll(gsmboxlib, "c_burst_size").value
C_TRAIN_SEQ_LENGTH = ctypes.c_uint.in_dll(gsmboxlib, "c_train_seq_length").value
C_GSM_SYMBOL_RATE = ctypes.c_float.in_dll(gsmboxlib, "c_gsm_symbol_rate").value
C_TRAINING_POS =ctypes.c_uint.in_dll(gsmboxlib, "c_training_pos").value

gsmboxlib.create_lpfir_filter.argtypes = [
    ctypes.c_double,
    ctypes.c_uint,
    np.ctypeslib.ndpointer(dtype=np.float32, ndim=1, flags='C_CONTIGUOUS')
]
gsmboxlib.create_lpfir_filter.restype = ctypes.c_uint

gsmboxlib.get_train_seq_iq.argtypes = [
    ctypes.c_uint,
    ctypes.c_uint,
    np.ctypeslib.ndpointer(dtype=np.complex64, ndim=1, flags='C_CONTIGUOUS')
]
gsmboxlib.get_train_seq_iq.restype = ctypes.c_uint

gsmboxlib.get_burst_start.argtypes = [
    ctypes.c_uint,
    ctypes.c_double,
    ctypes.c_uint,
    np.ctypeslib.ndpointer(dtype=np.complex64, ndim=1, flags='C_CONTIGUOUS')
]
gsmboxlib.get_burst_start.restype = ctypes.c_uint

gsmboxlib.get_burst_offset.argtypes = [
    ctypes.c_uint,
    np.ctypeslib.ndpointer(dtype=np.complex64, ndim=1, flags='C_CONTIGUOUS'),
    np.ctypeslib.ndpointer(dtype=np.complex64, ndim=1, flags='C_CONTIGUOUS')
]
gsmboxlib.get_burst_offset.restype = ctypes.c_uint

gsmboxlib.get_imp_resp_old.argtypes = [
    ctypes.c_uint,
    np.ctypeslib.ndpointer(dtype=np.complex64, ndim=1, flags='C_CONTIGUOUS'),
    np.ctypeslib.ndpointer(dtype=np.complex64, ndim=1, flags='C_CONTIGUOUS'),
    ctypes.c_uint,
    np.ctypeslib.ndpointer(dtype=np.complex64, ndim=1, flags='C_CONTIGUOUS')
]
gsmboxlib.get_imp_resp_old.restype = ctypes.c_uint

gsmboxlib.get_imp_resp.argtypes = [
    ctypes.c_uint,
    np.ctypeslib.ndpointer(dtype=np.complex64, ndim=1, flags='C_CONTIGUOUS'),
    np.ctypeslib.ndpointer(dtype=np.complex64, ndim=1, flags='C_CONTIGUOUS'),
    ctypes.c_uint,
    np.ctypeslib.ndpointer(dtype=np.complex64, ndim=1, flags='C_CONTIGUOUS')
]
gsmboxlib.get_imp_resp.restype = ctypes.c_uint

gsmboxlib.mafi_burst.argtypes = [
    ctypes.c_uint,
    np.ctypeslib.ndpointer(dtype=np.complex64, ndim=1, flags='C_CONTIGUOUS'),
    np.ctypeslib.ndpointer(dtype=np.complex64, ndim=1, flags='C_CONTIGUOUS'),
    ctypes.c_uint,
    np.ctypeslib.ndpointer(dtype=np.complex64, ndim=1, flags='C_CONTIGUOUS')
]
gsmboxlib.mafi_burst.restype = ctypes.c_uint

gsmboxlib.demod_burst.argtypes = [
    ctypes.c_uint,
    np.ctypeslib.ndpointer(dtype=np.complex64, ndim=1, flags='C_CONTIGUOUS'),
    np.ctypeslib.ndpointer(dtype=np.complex64, ndim=1, flags='C_CONTIGUOUS'),
    ctypes.c_uint,
    np.ctypeslib.ndpointer(dtype=ctypes.c_byte, ndim=1, flags='C_CONTIGUOUS')
]
gsmboxlib.demod_burst.restype = ctypes.c_uint

gsmboxlib.filter_data.argtypes = [
    ctypes.c_uint,
    np.ctypeslib.ndpointer(dtype=np.float32, ndim=1, flags='C_CONTIGUOUS'),
    ctypes.c_double,
    ctypes.c_uint,
    np.ctypeslib.ndpointer(dtype=np.complex64, ndim=1, flags='C_CONTIGUOUS'),
    np.ctypeslib.ndpointer(dtype=np.complex64, ndim=1, flags='C_CONTIGUOUS')
]
gsmboxlib.filter_data.restype = ctypes.c_uint

gsmboxlib.rotate_data.argtypes = [
    ctypes.c_double,
    ctypes.c_uint,
    np.ctypeslib.ndpointer(dtype=np.complex64, ndim=1, flags='C_CONTIGUOUS'),
    np.ctypeslib.ndpointer(dtype=np.complex64, ndim=1, flags='C_CONTIGUOUS')
]
gsmboxlib.rotate_data.restype = ctypes.c_uint

gsmboxlib.get_phase_inc.argtypes = [
    ctypes.c_uint,
    ctypes.c_float
]
gsmboxlib.get_phase_inc.restype = ctypes.c_double

gsmboxlib.get_freq_offset.argtypes = [
    ctypes.c_uint,
    ctypes.c_double
]
gsmboxlib.get_freq_offset.restype = ctypes.c_float

gsmboxlib.decode_bursts.argtypes = [
    np.ctypeslib.ndpointer(dtype=ctypes.c_byte, ndim=1, flags='C_CONTIGUOUS'),
    ctypes.c_uint,
    np.ctypeslib.ndpointer(dtype=ctypes.c_byte, ndim=1, flags='C_CONTIGUOUS')
]
gsmboxlib.decode_bursts.restype = ctypes.c_uint

#--- ----------------------------------------------------------------------------

#-------------------------------------------------------------------------------
class DataCursor(object):
    """A simple data cursor widget that displays the x,y location of a
    matplotlib artist when it is selected."""
    def __init__(self, artists, tolerance=5, offsets=(-20, 20), template='x: %0.2f\ny: %0.2f', display_all=False):
        """Create the data cursor and connect it to the relevant figure.
        "artists" is the matplotlib artist or sequence of artists that will be
            selected.
        "tolerance" is the radius (in points) that the mouse click must be
            within to select the artist.
        "offsets" is a tuple of (x,y) offsets in points from the selected
            point to the displayed annotation box
        "template" is the format string to be used. Note: For compatibility
            with older versions of python, this uses the old-style (%)
            formatting specification.
        "display_all" controls whether more than one annotation box will
            be shown if there are multiple axes.  Only one will be shown
            per-axis, regardless.
        """
        self.template = template
        self.offsets = offsets
        self.display_all = display_all
        if not np.iterable(artists):
            artists = [artists]
        self.artists = artists
        self.axes = tuple(set(art.axes for art in self.artists))
        self.figures = tuple(set(ax.figure for ax in self.axes))

        self.annotations = {}
        for ax in self.axes:
            self.annotations[ax] = self.annotate(ax)

        for artist in self.artists:
            artist.set_picker(tolerance)
        for fig in self.figures:
            fig.canvas.mpl_connect('pick_event', self)

    def annotate(self, ax):
        """Draws and hides the annotation box for the given axis "ax"."""
        annotation = ax.annotate(self.template, xy=(0, 0), ha='right',
                xytext=self.offsets, textcoords='offset points', va='bottom',
                bbox=dict(boxstyle='round,pad=0.5', fc='yellow', alpha=0.5),
                arrowprops=dict(arrowstyle='->', connectionstyle='arc3,rad=0')
                )
        annotation.set_visible(False)
        return annotation

    def __call__(self, event):
        """Intended to be called through "mpl_connect"."""
        # Rather than trying to interpolate, just display the clicked coords
        # This will only be called if it's within "tolerance", anyway.
        x, y = event.mouseevent.xdata, event.mouseevent.ydata
        annotation = self.annotations[event.artist.axes]
        if x is not None:
            if not self.display_all:
                # Hide any other annotation boxes...
                for ann in self.annotations.values():
                    ann.set_visible(False)
            # Update the annotation in the current axis..
            annotation.xy = x, y
            annotation.set_text(self.template % (x, y))
            annotation.set_visible(True)
            event.canvas.draw()
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
def plot_sig(a_name, a_sig):
    plt.figure(a_name)
    ylines, = plt.plot(a_sig)
    plt.xlabel("Samples")
    DataCursor(artists=[ylines], template='x: %0.0f\ny: %0.3f')
    plt.xlabel("Samples")
    plt.grid(True)
    plt.draw()
#    plt.show()
    return
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
def plot_sig2(a_name, a_sig1, a_sig2):
    plt.figure(a_name)
    ylines1, = plt.plot(a_sig1)
    ylines2, = plt.plot(a_sig2)
    DataCursor(artists=[ylines1, ylines2], template='x: %0.0f\ny: %0.3f')
    plt.xlabel("Samples")
    plt.grid(True)
    plt.draw()
#    plt.show()
    return
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
def plot_iq(a_name, a_sig_iq):
    plt.figure(a_name)
    ylines, = plt.plot(np.real(a_sig_iq), np.imag(a_sig_iq))
    DataCursor(artists=[ylines], template='x: %0.0f\ny: %0.3f')
    plt.xlabel("IQ")
    plt.grid(True)
    plt.draw()
#    plt.show()
    return
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
def plot_cfft(a_name, a_Fs, a_sig, a_plot_phase = False):
    N = len(a_sig)
    freq = np.arange(N)*a_Fs/N -a_Fs/2

    X = 1/N*np.fft.fftshift(np.fft.fft(a_sig, n=N))
    yf = 20*np.log10(np.abs(X))
    plt.figure(a_name)
    yflines, = plt.plot(freq, yf)
    DataCursor(artists=[yflines], template='x: %0.0f\ny: %0.3f')
    plt.grid(True)
    plt.suptitle('FFT (%u points)' % N)

    if (a_plot_phase == True):
        # Filter out phase noise
        X2=X
        threshold = max(np.abs(X))/1000
        #X2[np.abs(X)<threshold] = 0
        #X2 = np.where(X < threshold, 0, X)  # set value '0' where condition is met
        ph = np.arctan2(np.imag(X2), np.real(X2))*180/np.pi
        plt.figure("Phase spectrum")
        phlines, = plt.plot(freq, ph)
        DataCursor(artists=[phlines], template='x: %0.0f\ny: %0.3f')
        plt.grid(True)
        plt.suptitle('FFT (%u points)' % N)

    plt.draw()

    return
#-------------------------------------------------------------------------------

def plot_fft(a_name, a_fs, a_sig):
    N = len(a_sig)

    window = np.hanning(N)

    xfb = a_sig[0:N] * window
    FFT = np.fft.fft(xfb) / N  # Normalize
    # Adjust power from symetric components.
    # Adjust power from hanning window coefficient=0.5.
    yf = (np.abs(FFT[0:N // 2]) * 2) * 2
    # yf[0] /= 4 # Correct for DC component adjustmment?

    freq = np.arange(N // 2) * a_fs / N
    plt.figure(a_name)
    flines, = plt.plot(freq, 20 * np.log10(yf))
    DataCursor(artists=[flines], template='x: %0.0f\ny: %0.3f')
    plt.grid(True)
    plt.suptitle('FFT (%u points)' % N)
#    plt.autoscale(False)
#    plt.ylim([-140, 0])
#    datacursor(flines, formatter=myFormatter)
    plt.draw()
    return

#-------------------------------------------------------------------------------
def moving_average1(x, w):
    out = np.zeros(w-1, dtype=x.dtype)
    out = np.append(out, x)
    return np.convolve(out, np.ones(w), 'valid') / w
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
def moving_average2(a, n=3):
    out = np.zeros(n-1, dtype=a.dtype)
    out = np.append(out, a)
    ret = np.cumsum(out)
    ret[n:] = ret[n:] - ret[:-n]
    return ret[n - 1:] / n
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
def get_lpfir(a_samp_rate, a_len=2000):
    fir_coeffs = np.zeros(a_len, dtype=np.float32)
    fir_coeffs_len = len(fir_coeffs)
    fir_len = gsmboxlib.create_lpfir_filter(a_samp_rate, fir_coeffs_len, fir_coeffs)
    if fir_len > fir_coeffs_len:
        return None
    fir_coeffs = np.resize(fir_coeffs, fir_len)

    return fir_coeffs
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
def get_train_seq_iq(a_id, a_len=C_TRAIN_SEQ_LENGTH):
    train_seq_iq = np.zeros(a_len, dtype=np.complex64)
    train_seq_len = len(train_seq_iq)
    train_len = gsmboxlib.get_train_seq_iq(a_id, train_seq_len, train_seq_iq)
    if train_len > train_seq_len:
        return None
    train_seq_iq = np.resize(train_seq_iq, train_len)

    return train_seq_iq
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
def get_burst_start(a_osr, a_mwa_thr, a_len, a_sig):
    burst_start = gsmboxlib.get_burst_start(a_osr, a_mwa_thr, a_len, a_sig)
    return burst_start
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
def get_burst_offset(a_osr, a_train_seq, a_sig):
    burst_offs = gsmboxlib.get_burst_offset(a_osr, a_train_seq, a_sig)
    return burst_offs
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
def get_imp_resp(a_osr, a_train_seq, a_sig):
    imp_resp = np.zeros(a_osr*C_CHAN_IMP_RESP_LENGTH, dtype=np.complex64)
    imp_resp_len = len(imp_resp)
    resp_len = gsmboxlib.get_imp_resp(a_osr, a_train_seq, a_sig, imp_resp_len, imp_resp)

    if resp_len > imp_resp_len:
        return None
    imp_resp = np.resize(imp_resp, resp_len)

    return imp_resp
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
def mafi_burst(a_osr, a_imp_resp, a_sig):
    filtered = np.zeros(C_BURST_SIZE, dtype=np.complex64)
    filtered_len = len(filtered)
    resp_len = gsmboxlib.mafi_burst(a_osr, a_imp_resp, a_sig, filtered_len, filtered)

    if resp_len > filtered_len:
        return None
    filtered = np.resize(filtered, resp_len)

    return filtered
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
def demod_burst(a_osr, a_imp_resp, a_filtered):
    binary_out = np.zeros(C_BURST_SIZE, dtype=ctypes.c_byte)
    binary_out_len = len(binary_out)
    resp_len = gsmboxlib.demod_burst(a_osr, a_imp_resp, a_filtered, binary_out_len, binary_out)

    if resp_len > binary_out_len:
        return None
    binary_out = np.resize(binary_out, resp_len)

    return binary_out
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
def filter_data(a_fir_coeffs, a_phase_inc, a_sig):
    sig_len = len(a_sig)
    fir_len = len(a_fir_coeffs)
    data_out = np.zeros((sig_len+fir_len), dtype=np.complex64)
    data_out_len = len(data_out)
    resp_len = gsmboxlib.filter_data(fir_len, a_fir_coeffs, a_phase_inc, sig_len, a_sig, data_out)

    if resp_len > data_out_len:
        return None
    data_out = np.resize(data_out, resp_len)

    return data_out
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
def rotate_data(a_phase_inc, a_sig):
    sig_len = len(a_sig)
    data_out = np.zeros((sig_len), dtype=np.complex64)
    data_out_len = len(data_out)
    resp_len = gsmboxlib.rotate_data(a_phase_inc, sig_len, a_sig, data_out)

    if resp_len > data_out_len:
        return None
    data_out = np.resize(data_out, resp_len)

    return data_out
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
def get_phase_inc(a_osr, a_freq_offset):
    phase_inc = gsmboxlib.get_phase_inc(a_osr, a_freq_offset)
    return phase_inc
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
def get_freq_offset(a_osr, a_phase_inc):
    freq_offset = gsmboxlib.get_freq_offset(a_osr, a_phase_inc)
    return freq_offset
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
def decode_bursts(a_bursts_data):
    result_out = np.zeros(256, dtype=ctypes.c_byte)
    result_out_len = len(result_out)
    resp_len = gsmboxlib.decode_bursts(a_bursts_data, result_out_len, result_out)

    if resp_len > result_out_len:
        return None
    result_out = np.resize(result_out, resp_len)

    return result_out
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
def print_binary_out(a_burst_data):
    idx = 0
    print('[', end='')
    for i in range(3):
        print('%d' % (a_burst_data[idx]), end='')
        idx = idx+1
    print(']', end='')

    print('[', end='')
    for i in range(57):
        print('%d' % (a_burst_data[idx]), end='')
        idx = idx + 1
    print(']', end='')

    print('[', end='')
    for i in range(1):
        print('%d' % (a_burst_data[idx]), end='')
        idx = idx + 1
    print(']', end='')

    print('[', end='')
    for i in range(26):
        print('%d' % (a_burst_data[idx]), end='')
        idx = idx + 1
    print(']', end='')

    print('[', end='')
    for i in range(1):
        print('%d' % (a_burst_data[idx]), end='')
        idx = idx + 1
    print(']', end='')

    print('[', end='')
    for i in range(57):
        print('%d' % (a_burst_data[idx]), end='')
        idx = idx + 1
    print(']', end='')

    print('[', end='')
    for i in range(3):
        print('%d' % (a_burst_data[idx]), end='')
        idx = idx+1
    print(']', end='')

    print('')

    return
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
def print_result_message(a_result_msg):
    idx = 0
    print('[', end='')
    for i in range(len(a_result_msg)):
        print(' %02x' % (np.uint8(a_result_msg[idx])), end='')
        idx = idx+1
    print(' ]')
    return
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
def get_bursts_seq(a_osr, a_raw_sig, a_burst_chunk_size, a_bursts, a_start_idx, a_train_seq_iq1, a_lpfir_coeffs, a_phase_inc):

    bursts_seq = np.array([], dtype=ctypes.c_byte)
    for i in range(4):
        burst_start = a_bursts[a_start_idx+i][0]
        burst_data = a_raw_sig[burst_start:burst_start+a_burst_chunk_size].copy()
        burst_fir = filter_data(a_lpfir_coeffs, a_phase_inc, burst_data)
        burst_fir = burst_fir[len(a_lpfir_coeffs)//2:len(a_lpfir_coeffs)//2+a_burst_chunk_size].copy()
    #    plot_sig("BurstFIR", np.abs(burst_fir))

        burst_offset = get_burst_offset(a_osr, a_train_seq_iq1, burst_fir)

        burst = burst_fir[burst_offset:burst_offset+(C_BURST_SIZE*a_osr)]
        train_burst = burst[C_TRAINING_POS * a_osr:(C_TRAINING_POS * a_osr) + C_TRAIN_SEQ_LENGTH * a_osr].copy()
        burst_impresp = get_imp_resp(a_osr, a_train_seq_iq1, burst)
        burst_mafi = mafi_burst(a_osr, burst_impresp, burst)
        binary_out = demod_burst(a_osr, burst_impresp, burst_mafi)
        print_binary_out(binary_out)
        bursts_seq = np.append(bursts_seq, binary_out)

    return bursts_seq
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
def main(argv, filtered_burst=None):

    mwa_threshold = 0.3
    osr = 8
    samp_rate = C_GSM_SYMBOL_RATE * osr
    lpfir_coeffs = get_lpfir(samp_rate)
    freq_offs = -2150.00
    phase_inc = get_phase_inc(osr, freq_offs)
    freq_offs = get_freq_offset(osr, phase_inc)

    raw_sig = np.fromfile("./input_raw.216Mhz.arfcn1.ul.gain_7.cf64", np.complex64)

    burst_chunk_size = (C_BURST_SIZE + 8) * osr
    raw_sig_abs_mva = moving_average2(np.abs(raw_sig), burst_chunk_size)

    global_max = 0
    bursts = []
    next_start = 0
    while ((next_start + burst_chunk_size) < len(raw_sig_abs_mva)):
        index = next((idx for idx, val in np.ndenumerate(raw_sig_abs_mva[next_start:]) if val > mwa_threshold), None)
        if index == None:
            break
        burst_start = index[0] + next_start
        max_val = raw_sig_abs_mva[burst_start]
        while max_val <= raw_sig_abs_mva[burst_start + 1]:
            burst_start = burst_start + 1
            max_val = raw_sig_abs_mva[burst_start]

        if max_val > global_max:
            global_max = max_val

        bursts.append(((burst_start - (C_BURST_SIZE + 8) * osr), max_val))
        next_start = burst_start + burst_chunk_size

    start_idx = 1
    train_seq_iq1 = get_train_seq_iq(1)
#    plot_sig("Burst0", np.abs(raw_sig[bursts[start_idx+0][0]:bursts[start_idx+0][0]+burst_chunk_size]))
#    plot_sig("Burst1", np.abs(raw_sig[bursts[start_idx+1][0]:bursts[start_idx+1][0]+burst_chunk_size]))
#    plot_sig("Burst2", np.abs(raw_sig[bursts[start_idx+2][0]:bursts[start_idx+2][0]+burst_chunk_size]))
#    plot_sig("Burst3", np.abs(raw_sig[bursts[start_idx+3][0]:bursts[start_idx+3][0]+burst_chunk_size]))

    bursts_seq = get_bursts_seq(osr, raw_sig, burst_chunk_size, bursts, start_idx, train_seq_iq1, lpfir_coeffs, get_phase_inc(osr, freq_offs))
    result = decode_bursts(bursts_seq)
    print_result_message(result)

    plt.show()

    return
#-------------------------------------------------------------------------------

if __name__ == "__main__":
    main(sys.argv[1:])

