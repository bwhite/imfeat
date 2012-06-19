
try:
    import unittest2 as unittest
except ImportError:
    import unittest

import imfeat
import cv2
import os
import numpy as np
import time


class Test(unittest.TestCase):

    def setUp(self):
        pass

    def tearDown(self):
        pass

    def test_save_lena(self):
        feat = imfeat.HOGLatent(8)
        image_input = imfeat.convert_image(cv2.imread('test_images/mosaic_001_01.jpg'), feat.MODES)#lena.jpg
        num_eq = 0
        num_neq = 0
        try:
            os.makedirs('out/latent/')
        except OSError:
            pass
        for x in range(260, 261):
            #image_input = imfeat.resize_image(image_input, x, x)
            for sz in range(1, 6):
                print(sz)
                sz = 2 ** sz
                num_blocks = (np.floor(np.asfarray(image_input.shape[:2]) / float(sz) + .5) - 2)
                print(num_blocks)
                if any(num_blocks <= 0):
                    continue
                #effective_size = (np.floor(np.asfarray(image_input.shape[:2]) / float(sz) + .5)) * sz
                feat = imfeat.HOGLatent(sz)
                im = image_input.copy()
                out = feat.make_feature_mask(im)
                print('Dims[%d]' % out.shape[2])
                for i in range(out.shape[2]):
                    out_s = out[:, :, i]
                    print(np.min(out_s))
                    print(np.max(out_s))
                    print('sz[%s]M[%s] m[%s]' % (sz, np.max(out_s), np.min(out_s)))
                    out_s = np.array(255 * (out_s - np.min(out_s)) / (np.max(out_s) - np.min(out_s) + .000000001), dtype=np.uint8)
                    cv2.imwrite('out/latent/lena-hog-%.3d-%.3d.png' % (sz, i), out_s)
                y, x = np.random.randint(0, im.shape[0]), np.random.randint(0, im.shape[1])
                im[y, x, :] += 100
                out2 = feat.make_feature_mask(im)
                if out[y, x, :].tolist() == out2[y, x, :].tolist():
                    num_eq += 1
                    print('-----------%s' % str((num_eq, num_neq)))
                else:
                    num_neq += 1
                
            #try:
            #    os.makedirs('out')
            #except OSError:
            #    pass
            #cv2.imwrite('out/lena_lbp.jpg', out)


if __name__ == '__main__':
    unittest.main()
