import numpy as np
from scipy.io.wavfile import read as wavread

x = np.loadtxt("out.txt")
fs, audio = wavread("test.wav")
audio = audio / (2 ** 15)
print("Error:", np.mean(np.abs(x - audio)))