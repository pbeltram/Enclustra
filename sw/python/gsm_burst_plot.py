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
def plot_sig3(a_name, a_sig1, a_sig2, a_sig3):
    plt.figure(a_name)
    ylines1, = plt.plot(a_sig1)
    ylines2, = plt.plot(a_sig2)
    ylines3, = plt.plot(a_sig3)
    DataCursor(artists=[ylines1, ylines2, ylines3], template='x: %0.0f\ny: %0.3f')
    plt.xlabel("Samples")
    plt.grid(True)
    plt.draw()
    return
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
def main(argv):
    name = "./burst_data/burst_003"
    sig = np.fromfile(name + ".cf64", np.complex64)
    sig_filtered = np.fromfile(name + "-filtered.cf64", np.complex64)
    sig_impresp = np.fromfile(name + "-impresp.cf64", np.complex64)

    sig_abs = np.abs(sig)
    sig_filtered_abs = abs(sig_filtered)
    sig_impresp_abs = abs(sig_impresp)

    plot_sig3("Sig", sig_abs, np.real(sig), np.imag(sig))
    plot_sig3("Filtered", sig_filtered_abs, np.real(sig_filtered), np.imag(sig_filtered))
    plot_sig3("ImpResp", sig_impresp_abs, np.real(sig_impresp), np.imag(sig_impresp))

    plt.show()

    return
#-------------------------------------------------------------------------------

if __name__ == "__main__":
    main(sys.argv[1:])

