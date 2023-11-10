import numpy as np
import matplotlib.pyplot as plt
from scipy.signal import freqz

# FIR filter coefficients as provided
fir_coeffs = np.array(
    [
        -0.0018225230,
        -0.0015879294,
        0.0000000000,
        +0.0036977508,
        +0.0080754303,
        +0.0085302217,
        -0.0000000000,
        -0.0173976984,
        -0.0341458607,
        -0.0333591565,
        +0.0000000000,
        +0.0676308395,
        +0.1522061835,
        +0.2229246956,
        +0.2504960933,
        +0.2229246956,
        +0.1522061835,
        +0.0676308395,
        +0.0000000000,
        -0.0333591565,
        -0.0341458607,
        -0.0173976984,
        -0.0000000000,
        +0.0085302217,
        +0.0080754303,
        +0.0036977508,
        +0.0000000000,
        -0.0015879294,
        -0.0018225230,
    ]
)

# Sample rate for the system
sample_rate = 48000
nyq_rate = sample_rate / 2.0

# Plot the FIR filter coefficients
plt.figure(1)
plt.plot(fir_coeffs, "bo-", linewidth=2)
plt.title("Filter Coefficients (%d taps)" % len(fir_coeffs))
plt.grid(True)

# Calculate the frequency response of the FIR filter
w, h = freqz(fir_coeffs, worN=8000)

# Plot the magnitude response of the filter
plt.figure(2)
plt.plot((w / np.pi) * nyq_rate, np.abs(h), linewidth=2)
plt.xlabel("Frequency (Hz)")
plt.ylabel("Gain")
plt.title("Frequency Response")
plt.ylim(-0.05, 1.05)
plt.grid(True)

# Upper inset plot showing the passband
ax1 = plt.axes([0.42, 0.6, 0.45, 0.25])
plt.plot((w / np.pi) * nyq_rate, np.abs(h), linewidth=2)
plt.xlim(0, 8.0)
plt.ylim(0.9985, 1.001)
plt.grid(True)

# Lower inset plot showing the stopband
ax2 = plt.axes([0.42, 0.25, 0.45, 0.25])
plt.plot((w / np.pi) * nyq_rate, np.abs(h), linewidth=2)
plt.xlim(12.0, 20.0)
plt.ylim(0.0, 0.0025)
plt.grid(True)

plt.show()
