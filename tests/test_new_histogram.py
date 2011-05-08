import imfeat
import cv
import unittest


class Test(unittest.TestCase):

    def test_hist(self):
        img = cv.LoadImage('test_images/lena.jpg')
        feat = imfeat.Histogram('rgb', [0, 0, 0], [1, 1, 1], [8, 8, 8])
        a = imfeat.compute(imfeat.histogram_joint, img)[0]
        b = imfeat.compute(feat, img)[0]
        a = a.reshape(8, 8, 8).T.ravel()
        self.assertEqual(a.tolist(), b.tolist())

    def test_hist_planar(self):
        img = cv.LoadImage('test_images/lena.jpg')
        feat = imfeat.Histogram('rgb', [0, 0, 0], [1, 1, 1], [8, 8, 8], style='planar')
        b = imfeat.compute(feat, img)[0]
        print(b)


if __name__ == '__main__':
    unittest.main()
