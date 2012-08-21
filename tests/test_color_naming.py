try:
    import unittest2 as unittest
except ImportError:
    import unittest
import imfeat
import cv2
class Test(unittest.TestCase):

    def setUp(self):
        pass

    def tearDown(self):
        pass

    def test_name(self):
        img = cv2.imread('test_images/lena.jpg')
        cn = imfeat.ColorNaming()
        print cn.make_feature_mask(img).shape

if __name__ == '__main__':
    unittest.main()
