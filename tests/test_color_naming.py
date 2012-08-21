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
        img = cv2.imread('test_images/dice_rgba.png')
        cn = imfeat.ColorNaming(size=5)
        print cn.make_feature_mask(img).shape
        print cn.color_names[np.argmax(cn(img).reshape(5, 5, 11), 2)]

if __name__ == '__main__':
    unittest.main()
