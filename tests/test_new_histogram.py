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
import numpy as np
modes = ['rgb', 'bgr', 'hls', 'hsv', 'lab', 'luv', 'xyz', 'ycrcb']

class Test(unittest.TestCase):

    def test_hist(self):
        img = cv.LoadImage('test_images/lena.jpg')
        feat = imfeat.Histogram('rgb', num_bins=8)
        feat2 = imfeat.PyramidHistogram(mode='rgb', num_bins=8, levels=1)
        np.testing.assert_equal(feat(img), feat2(img))
        out = imfeat.PyramidHistogram(mode='lab', num_bins=[4, 11, 11], levels=4)(img)
        print(out)
        print(out.shape)
        

    def test_hist_planar(self):
        img = cv.LoadImage('test_images/lena.jpg')
        for i in range(512):
            for j in range(512):
                img[i, j] = (random.randint(0, 255), random.randint(0, 255), random.randint(0, 255))
        for mode in modes:
            print(mode)
            feat = imfeat.Histogram(mode, style='planar')
            b = feat(img)
            print(b)


if __name__ == '__main__':
    unittest.main()
