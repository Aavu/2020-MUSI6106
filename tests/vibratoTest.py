import numpy as np
from scipy.io.wavfile import read as wavread
from scipy.io.wavfile import write as wavwrite
from matplotlib import pyplot as plt

def filter(x, fs, modFreq, width):
    y = np.zeros_like(x)
    dly = 0.02
    DLY = int(np.floor(dly * fs))
    WIDTH = int(np.floor(width * fs))
    MODFREQ = modFreq*1.0 / fs
    L = 2 + DLY + WIDTH * 2
    delayline = np.zeros(L)
    for n in range(len(x)-1):
        mod = (1 + np.sin((MODFREQ * 2 * np.pi * n) - (np.pi)))*0.5
        tap = WIDTH * mod
        i = int(np.floor(tap))
        frac = tap - i
        delayline[1:] = delayline[:-1]
        delayline[0] = x[n]
        y[n] = (delayline[i + 1] * frac) + (delayline[i] * (1 - frac))
    return y


fs, audio = wavread("tests/sine.wav")
_, cpp_filter = wavread("tests/sine_cpp.wav")
audio = audio / (2 ** 15)
cpp_filter = cpp_filter / (2 ** 15)
out = filter(audio, fs, 5.0, .02) # sine
wavwrite("outpy.wav", fs, out)
print(np.mean(np.abs(out[4000:]) - np.abs(cpp_filter[4000:])))
