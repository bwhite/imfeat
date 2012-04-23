import imfeat
import cv2
import unittest


class Test(unittest.TestCase):

    def test_hist(self):
        img = cv2.imread('test_images/lena.jpg')
        feat = imfeat.Autocorrelogram([1, 3, 5, 7])
        b = feat(img)
        print(b)


if __name__ == '__main__':
    unittest.main()
