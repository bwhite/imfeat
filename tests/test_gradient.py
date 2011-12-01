try:
    import unittest2 as unittest
except ImportError:
    import unittest

import imfeat
import cv2
import os
import numpy as np


class Test(unittest.TestCase):

    def setUp(self):
        pass

    def tearDown(self):
        pass

    def test_save_lena(self):
        feat = imfeat.GradientHistogram()
        out = feat.make_feature_mask(imfeat.convert_image(cv2.imread('test_images/lena.jpg'), feat.MODES))
        try:
            os.makedirs('out')
        except OSError:
            pass
        print('m[%s]M[%s]' % (np.min(out), np.max(out)))
        cv2.imwrite('out/lena_gradient.jpg', np.array(out * 255, dtype=np.uint8))

if __name__ == '__main__':
    unittest.main()
