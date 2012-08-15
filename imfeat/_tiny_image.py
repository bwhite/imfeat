import imfeat
import numpy as np


class TinyImage(imfeat.BaseFeature):

    def __init__(self, size=32, mode='bgr'):
        self.mode = mode
        super(TinyImage, self).__init__({'type': 'numpy', 'dtype': 'uint8', 'mode': mode})
        self.size = size

    def __call__(self, image):
        image = self.convert(image)
        return np.asfarray(imfeat.resize_image(image, self.size, self.size).ravel()) / 255.
