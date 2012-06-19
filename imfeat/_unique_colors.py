import imfeat
import numpy as np


class UniqueColors(imfeat.BaseFeature):

    def __init__(self):
        super(UniqueColors, self).__init__({'type': 'numpy', 'dtype': 'uint8', 'mode': 'bgr'})

    def __call__(self, image):
        image = self.convert(image)
        image = np.array(image, dtype=np.uint32).reshape(image.shape[0] * image.shape[1], image.shape[2])
        u = len(np.unique(image[:, 0] + image[:, 1] * 256 + image[:, 2] * 256 ** 2)) 
        return np.asfarray([u / float(256 ** 3), u / float(image.shape[0])])
