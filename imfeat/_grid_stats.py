import imfeat
import cv2
import numpy as np


class GridStats(imfeat.BaseFeature):

    def __init__(self, image_size=16):
        super(GridStats, self).__init__({'type': 'numpy', 'dtype': 'uint8', 'mode': 'gray'})
        self._image_size = image_size

    def _grid_features(self, image):
        return np.hstack([np.mean(image, 0),
                          np.max(image, 0) - np.min(image, 0),
                          np.std(image, 0),
                          np.mean(image, 1),
                          np.max(image, 1) - np.min(image, 1),
                          np.std(image, 1)])

    def __call__(self, image):
        image = self.convert(image)
        image = np.asfarray(cv2.resize(image, (self._image_size, self._image_size))) / 255.
        sz2 = self._image_size / 2
        folded_image_h = (np.abs(image[sz2:, :] + image[:sz2-1:-1, :])) * .5
        folded_image_v = (np.abs(image[:, sz2:] + image[:, :sz2-1:-1])) * .5
        return np.hstack(map(self._grid_features, [image, folded_image_h, folded_image_v]))
