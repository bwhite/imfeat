import imfeat
import numpy as np


class TinyImage(imfeat.BaseFeature):

    def __init__(self, size=32):
        super(TinyImage, self).__init__({'type': 'numpy', 'dtype': 'uint8', 'mode': 'bgr'})
        self.size = size

    def __call__(self, image):
        image = self.convert(image)
        return np.asfarray(imfeat.resize_image(image, self.size, self.size,
                                               {'type': 'numpy', 'dtype': 'uint8', 'mode': 'bgr'}).ravel()) / 255.
