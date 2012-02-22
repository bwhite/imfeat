import imfeat
import numpy as np


class TinyImage(imfeat.BaseFeature):

    def __init__(self):
        super(TinyImage, self).__init__()

    def __call__(self, image):
        return np.asfarray(imfeat.resize_image(image, 32, 32, {'type': 'numpy', 'dtype': 'uint8', 'mode': 'bgr'}).ravel()) / 255.
