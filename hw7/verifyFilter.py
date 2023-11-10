import numpy as np
import matplotlib.pyplot as plt
from scipy.signal import lfilter

# Sample rate and test signal length
sample_rate = 48000
nsamples = 320

# Time array for the signal
t = np.arange(nsamples) / sample_rate

# Create a signal composed of 1 kHz and 15 kHz sine waves
signal = 0.8 * np.sin(2 * np.pi * 1e3 * t) + 0.2 * np.sin(2 * np.pi * 15e3 * t)

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

# Apply the FIR filter to the signal
filtered_signal = lfilter(fir_coeffs, 1.0, signal)

# Plot the original and filtered signals
plt.figure(figsize=(12, 6))

# Plot the original signal

plt.subplot(2, 1, 1)
plt.plot(t, signal, label="Original Signal")
plt.title("Original and Filtered Signals")
plt.xlabel("Sample point")
plt.ylabel("Amplitude")
plt.legend()
plt.grid(True)

# Plot the filtered signal
plt.subplot(2, 1, 2)
plt.plot(t, filtered_signal, label="Filtered Signal", color="red")
plt.xlabel("Sample point")
plt.ylabel("Amplitude")
plt.legend()
plt.grid(True)

plt.tight_layout()
plt.show()
