import numpy as np


class Dummy(object):

    def __init__(self, zeros):
        super(Dummy, self).__init__()
        self.zeros = zeros

    def __call__(self, image):
        return np.zeros(self.zeros)
