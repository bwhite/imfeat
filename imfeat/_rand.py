import numpy as np
import imfeat


class Random(imfeat.BaseFeature):

    def __init__(self, dim=2):
        super(Random, self).__init__()
        self._dim = dim
    
    def __call__(self, image):
        return np.random.random(self._dim)
