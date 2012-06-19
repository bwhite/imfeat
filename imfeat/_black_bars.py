import imfeat
import cv2
import numpy as np


class BlackBars(imfeat.BaseFeature):

    def __init__(self, image_size=64, black_thresh=20, mode='both'):
        super(BlackBars, self).__init__({'type': 'numpy', 'dtype': 'uint8', 'mode': 'gray'})
        self._image_size = image_size
        self._black_thresh = black_thresh
        self._mode = mode

    def find_bars(self, image, mode=None, symmetric=True):
        if mode is None:
            mode = self._mode
        image_size = tuple(np.array(image.shape[:2]))
        f = self(image, mode='both', image_size=image_size).tolist()
        x_f = f[:image_size[1]]
        try:
            x_min = x_f.index(0.)
            x_max = x_f[::-1].index(0.)
        except ValueError:
            x_max = x_min = 0
        else:
            if symmetric:
                x_min = x_max = min(x_min, x_max)
            x_max = image_size[1] - x_min
        y_f = f[image_size[1]:image_size[1] + image_size[0]]
        try:
            y_min = y_f.index(0.)
            y_max = y_f[::-1].index(0.)
        except ValueError:
            y_max = y_min = 0
        else:
            if symmetric:
                y_min = y_max = min(y_min, y_max)
            y_max = image_size[0] - y_min
        return y_min, y_max, x_min, x_max

    def __call__(self, image, mode=None, image_size=None):
        if mode is None:
            mode = self._mode
        if image_size is None:
            image_size = self._image_size
        if isinstance(image_size, int):
            image_size = (image_size, image_size)
        image = cv2.resize(self.convert(image), image_size[::-1])
        image = image <= self._black_thresh
        h2 = image_size[0] / 2
        w2 = image_size[1] / 2
        # If image_size is odd, then the last row/col is ignored
        image_h = np.logical_and(image[h2:2 * h2, :], image[:h2, :][::-1, :])
        image_v = np.logical_and(image[:, w2:2 * w2], image[:, :w2][:, ::-1])
        h, w = image_size
        if mode == 'both':
            return np.asfarray(np.hstack([np.sum(image, 0) == h,
                                          np.sum(image, 1) == w,
                                          np.sum(image_h, 1) == w,
                                          np.sum(image_v, 0) == h]))
        elif mode == 'vertical':
            return np.asfarray(np.hstack([np.sum(image, 0),
                                          np.sum(image_v, 0)]) == h)
        elif mode == 'horizontal':
            return np.asfarray(np.hstack([np.sum(image, 1),
                                          np.sum(image_h, 1)]) == w)
        else:
            raise ValueError('Unknown mode[%s]' % mode)
