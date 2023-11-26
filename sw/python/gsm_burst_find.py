#!/usr/bin/env python3

import sys
import numpy as np
import matplotlib
matplotlib.use('TKAgg')
import matplotlib.pyplot as plt

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
def plot_sig2(a_name, a_sig1, a_sig2):
    plt.figure(a_name)
    ylines1, = plt.plot(a_sig1)
    ylines2, = plt.plot(a_sig2)
    DataCursor(artists=[ylines1, ylines2], template='x: %0.0f\ny: %0.3f')
    plt.xlabel("Samples")
    plt.grid(True)
    plt.draw()
    return
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
def moving_average(a, n=3):
    out = np.zeros(n-1, dtype=a.dtype)
    out = np.append(out, a)
    ret = np.cumsum(out)
    ret[n:] = ret[n:] - ret[:-n]
    return ret[n - 1:] / n
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
def main(argv):
    samp_ratio = 1  # 1=1.083333, 2=2.166666, 4=4.333333
    sig = np.fromfile("../gsm_passive_receiver_file/iq-ul-4bursts.cf64", np.complex64)

    osr = 4
    processed_chunk_size = int((3+57+1+26+1+57+3)*osr)*samp_ratio
    mwa_threshold = 0.025

    sig_abs = np.abs(sig)
    sig_abs_mva = moving_average(sig_abs, processed_chunk_size)
    np_mva_index, = np.where(sig_abs_mva > mwa_threshold)

    global_max = 0
    bursts = []
    next_start = 0
    while ((next_start + processed_chunk_size) < len(sig_abs_mva)):
        index = next((idx for idx, val in np.ndenumerate(sig_abs_mva[next_start:]) if val > mwa_threshold), None)
        if index == None:
            break
        burst_start = index[0] + next_start
        max_val = sig_abs_mva[burst_start]
        while max_val <= sig_abs_mva[burst_start+1]:
            burst_start = burst_start + 1
            max_val = sig_abs_mva[burst_start]

        if max_val > global_max:
            global_max = max_val

        bursts.append(((burst_start-((148+8)*osr)*samp_ratio), max_val))
        next_start = burst_start + processed_chunk_size

    burst_start = bursts[3][0]

    plot_sig2("Abs", sig_abs[burst_start-300:burst_start+processed_chunk_size*2], sig_abs_mva[burst_start-300:burst_start+processed_chunk_size*2])
    plot_sig2("IQ", np.real(sig[burst_start-300:burst_start+processed_chunk_size*2]), np.imag(sig[burst_start-300:burst_start+processed_chunk_size*2]))

    plt.show()

    return
#-------------------------------------------------------------------------------

if __name__ == "__main__":
    main(sys.argv[1:])

