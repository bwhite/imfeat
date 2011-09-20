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
        feat = imfeat.LBP()
        for x in range(10):
            out = feat.make_feature_mask(imfeat.convert_image(cv2.imread('test_images/lena.jpg'), feat.MODES), pool_radius=x)
            print(out.shape)
            out = out.reshape(out.shape[:2])
            try:
                os.makedirs('out')
            except OSError:
                pass
            cv2.imwrite('out/lena_lbp-%d.jpg' % x, out)

    def test_patterns(self):
        feat = imfeat.LBP()
        for in_str, out_val in [('[1,1,1;1,2,1;1,1,1]', 255),
                                ('[1,1,1;1,1,1;1,1,1]', 0),
                                ('[0,1,1;1,1,1;1,1,1]', 1),
                                ('[1,0,1;1,1,1;1,1,1]', 2),
                                ('[1,1,0;1,1,1;1,1,1]', 4),
                                ('[1,1,1;1,1,0;1,1,1]', 8),
                                ('[1,1,1;1,1,1;1,1,0]', 16),
                                ('[1,1,1;1,1,1;1,0,1]', 32),
                                ('[1,1,1;1,1,1;0,1,1]', 64),
                                ('[1,1,1;0,1,1;1,1,1]', 128),
                                ('[0,1,1;0,1,1;1,1,1]', 129)]:
            data = np.array(np.mat(in_str).A, dtype=np.uint8)
            expected = np.ones((3, 3), dtype=np.uint8) * out_val
            out = feat.make_feature_mask(imfeat.convert_image(data, feat.MODES))
            np.testing.assert_equal(out, expected)


if __name__ == '__main__':
    unittest.main()
