import imfeat
import numpy as np
import cv
import cv2


class GradientHistogram(imfeat.BaseFeature):

    def __init__(self, num_bins=8, *args, **kw):
        super(GradientHistogram, self).__init__(*args, **kw)
        self._hist_feat = imfeat.Histogram('gray', num_bins=num_bins)
        self.MODES = [('opencv', 'gray', 32)]

    def make_features(self, image_cv):
        image_cv = cv2.GaussianBlur(np.asarray(cv.GetMat(image_cv)), (3, 3), 0)
        dx = cv2.Sobel(image_cv, cv2.CV_32F, 1, 0)
        dy = cv2.Sobel(image_cv, cv2.CV_32F, 0, 1)
        image_cv = cv.fromarray(.5 * (np.abs(dx) + np.abs(dy)))
        return self._hist_feat.make_features(image_cv)
#b=np.array(np.random.random((10, 10)), dtype=np.float32)
#a=GradientHistogram().make_features(cv.GetImage(cv.fromarray(b)))
