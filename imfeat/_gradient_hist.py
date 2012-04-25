import imfeat
import numpy as np
import cv2


class GradientHistogram(imfeat.BaseFeature):

    def __init__(self, num_bins=8, *args, **kw):
        super(GradientHistogram, self).__init__({'type': 'numpy', 'dtype': 'float32', 'mode': 'gray'})
        self._hist_feat = imfeat.Histogram('gray', num_bins=num_bins)

    def make_feature_mask(self, image):
        image = self.convert(image)
        image = cv2.GaussianBlur(image, (3, 3), 0)
        dx = cv2.Sobel(image, cv2.CV_32F, 1, 0)
        dy = cv2.Sobel(image, cv2.CV_32F, 0, 1)
        return .25 * (np.abs(dx) + np.abs(dy))

    def __call__(self, image_cv):
        return self._hist_feat(self.make_feature_mask(image_cv))
