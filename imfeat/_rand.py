import numpy as np


class Random(object):

    def __init__(self, dim=2):
        self._dim = dim
    
    def __call__(self, image):
        return np.random.random(self._dim)
