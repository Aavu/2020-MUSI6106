import numpy as np
from scipy.io.wavfile import read as wavread

def filter(x, g=0.5, L=10):
    y = np.zeros_like(x)
    delayline = np.zeros(L)
    for n in range(len(x)):
        y[n] = x[n] + (g * delayline(-1))
        delayline[1:] = delayline[:-1]
        delayline[0] = x[n]
 
fs, audio = wavread("noise.wav")
# fs, out_c = wavread("out.wav")
# out_c = out_c / (2 ** 15)
audio = audio / (2 ** 15)
out = filter(audio, 1, 441)
# print("Error:", np.mean(np.abs(out - out_c)))