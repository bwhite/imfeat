try:
    import unittest2 as unittest
except ImportError:
    import unittest
import numpy as np
import cv
import os
import imfeat

class Test(unittest.TestCase):

    def setUp(self):
        pass

    def tearDown(self):
        pass

    def test_coord_gen_rect(self):
        vals = [np.asfarray([0, 0, 5, 5, 0, 1]),
                np.asfarray([0, 5, 5, 5, 0, 1]),
                np.asfarray([5, 0, 5, 5, 0, 1]),
                np.asfarray([5, 5, 5, 5, 0, 1])]
        print(list(imfeat.CoordGeneratorRect((10, 10), (5, 5), (5, 5))))
        for x, y in zip(vals, imfeat.CoordGeneratorRect((10, 10), (5, 5), (5, 5))):
            print(x)
            np.testing.assert_equal(x, y)

    def test_block_gen(self):
        img = cv.LoadImage('test_images/lena.jpg')
        bgen = imfeat.BlockGenerator(img, imfeat.CoordGeneratorRect,
                                     output_size=(200, 200), step_delta=(200, 200))
        try:
            os.makedirs('out')
        except OSError:
            pass
        for num, (x, y) in enumerate(bgen):
            cv.SaveImage('out/%.8d.jpg' % num, x)

    def test_block_gen_rot(self):
        img = cv.LoadImage('test_images/lena.jpg')
        bgen = imfeat.BlockGenerator(img, imfeat.CoordGeneratorRectRotate,
                                     output_size=(200, 200), step_delta=(200, 200), angle_steps=8)
        try:
            os.makedirs('out_rot')
        except OSError:
            pass
        for num, (x, y) in enumerate(bgen):
            cv.SaveImage('out_rot/%.8d.jpg' % num, x)


if __name__ == '__main__':
    unittest.main()
