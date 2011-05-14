import imfeat
import cv
import unittest

#    code = {'rgb': cv.CV_BGR2RGB,
#            'gray': cv.CV_BGR2GRAY,
#            'hls': cv.CV_BGR2HLS,
#            'hsv': cv.CV_BGR2HSV,
#            'lab': cv.CV_BGR2Lab,
#            'luv': cv.CV_BGR2Luv,
#            'xyz': cv.CV_BGR2XYZ,
#            'ycrcb': cv.CV_BGR2YCrCb}[mode]
import random
modes = ['rgb', 'bgr', 'hls', 'hsv', 'lab', 'luv', 'xyz', 'ycrcb']

class Test(unittest.TestCase):

    def _test_hist(self):
        img = cv.LoadImage('test_images/lena.jpg')
        feat = imfeat.Histogram('rgb', [0, 0, 0], [1, 1, 1], [8, 8, 8])
        a = imfeat.compute(imfeat.histogram_joint, img)[0]
        b = imfeat.compute(feat, img)[0]
        a = a.reshape(8, 8, 8).T.ravel()
        self.assertEqual(a.tolist(), b.tolist())

    def test_hist_planar(self):
        img = cv.LoadImage('test_images/lena.jpg')
        for i in range(512):
            for j in range(512):
                img[i, j] = (random.randint(0, 255), random.randint(0, 255), random.randint(0, 255))
        for mode in modes:
            print(mode)
            feat = imfeat.Histogram(mode, style='planar')
            b = imfeat.compute(feat, img)[0]
            print(b)


if __name__ == '__main__':
    unittest.main()
