import imfeat
import cv
import unittest


class Test(unittest.TestCase):

    def test_hist(self):
        img = cv.LoadImage('test_images/lena.jpg')
        feat = imfeat.Autocorrelogram([1, 3, 5, 7])
        b = imfeat.compute(feat, img)[0]
        print(b)


if __name__ == '__main__':
    unittest.main()
