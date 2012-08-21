try:
    import unittest2 as unittest
except ImportError:
    import unittest
import imfeat
import cv2
import numpy as np

class Test(unittest.TestCase):

    def setUp(self):
        pass

    def tearDown(self):
        pass

    def test_name(self):
        img = cv2.imread('test_images/opp_color_circle.png')
        cn = imfeat.ColorNaming(size=8)
        print cn.make_feature_mask(img).shape
        print cn.color_names[np.argmax(cn(img).reshape(8, 8, 11), 2)]

if __name__ == '__main__':
    unittest.main()
