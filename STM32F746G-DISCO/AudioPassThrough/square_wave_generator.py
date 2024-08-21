import numpy as np
import wave

# Parameters
f = 440.0  # Frequency of the square wave in Hz
Fs = 48000  # Sampling rate in Hz
T = 1.0    # Duration in seconds
A = 0.5    # Amplitude of the square wave

# Number of samples
N = int(Fs * T)

# Time vector
t = np.arange(N) / Fs

# Generate the square wave
square_wave = A * np.sign(np.sin(2 * np.pi * f * t))

# Convert to 16-bit PCM
pcm_wave = np.int16(32767 * square_wave)

print(pcm_wave)

# Save to a WAV file
with wave.open('square_wave.wav', 'w') as wf:
    wf.setnchannels(1)  # Mono
    wf.setsampwidth(2)  # 16-bit
    wf.setframerate(Fs)
    wf.writeframes(pcm_wave.tobytes())
