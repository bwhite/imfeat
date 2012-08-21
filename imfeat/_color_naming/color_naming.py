import imfeat
import numpy as np
import gzip
from . import __path__


class ColorNaming(imfeat.BaseFeature):

    def __init__(self, size=32):
        super(ColorNaming, self).__init__({'type': 'numpy', 'dtype': 'uint8', 'mode': 'bgr'})
        self.size = size
        self.w2c = np.array([map(float, x.split())[3:] for x in gzip.GzipFile(__path__[0] + '/data/w2c.txt.gz')])

    def make_feature_mask(self, image, converted=False):
        if not converted:
            image = self.convert(image)
        image = (image / 8).astype(np.int32)
        image = image[:, :, 0] + image[:, :, 1] * 32 + image[:, :, 0] * 1024
        return self.w2c[image, :]

    def __call__(self, image):
        image = self.convert(image)
        return self.make_feature_mask(imfeat.resize_image(image, self.size, self.size), converted=True).ravel()
