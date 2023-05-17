import numpy as np

def medianFilter(Origine, kernelShape):
    median = np.zeros(Origine.shape)
    padding_dimensions = int((kernelShape - 1) / 2)
    padded = np.pad(Origine, padding_dimensions, mode='constant')
    for i in range(Origine.shape[1]):
        for j in range(Origine.shape[0]):
            median[j,i] = np.median(padded[j:j+kernelShape, i:i+kernelShape])
    return median