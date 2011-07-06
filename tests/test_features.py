import unittest
import Image
import imfeat
import hashlib
import numpy as np
import scipy as sp
import scipy.io
import gzip
import pickle
import cv
import cv2


def load_from_umiacs(path, md5hash):
    import os
    import urllib
    name = os.path.basename(path)
    download = not os.path.exists(path)
    if os.path.exists(path) and md5hash:
        with open(path) as fp:
            if hashlib.md5(fp.read()).hexdigest() != md5hash:
                download = True
    if download:
        url = 'http://umiacs.umd.edu/~bwhite/%s' % name
        print('Downloading [%s]' % url)
        data = urllib.urlopen(url).read()
        with open(path, 'w') as fp:
            if md5hash:
                assert(md5hash == hashlib.md5(data).hexdigest())
            fp.write(data)


class TestFeatures(unittest.TestCase):
    
    def setUp(self):
        self.image_names = ['test_images/test1.jpg', 
                            'test_images/test2.jpg']

    def _histogram(self, hf):
        # Test the histogram module by loading each test image
        # and converting it to each possible mode.
        # Also check that the size of the returned histogram is as
        # expected.
        self._feat_hist_norm(hf)
        for fn in self.image_names:
            img = Image.open(fn)
            self.assertAlmostEquals(np.sum(hf(img)), 1.)
            h = imfeat.compute(hf, img)
            self.assertTrue(len(h) == 1)
            self.assertTrue(h[0].shape == (8*8*8,))

    def _run_all_images(self, feature):
        images = (Image.open(fn)
                  for fn in self.image_names)
        return ((imfeat.compute(feature, image), image)
                for image in images)

    def _feat_hist_zero(self, feature):
        img = np.array(np.zeros((50, 50)), dtype=np.uint8)
        self.assertEquals(feature(cv.GetImage(cv.fromarray(img)))[0], 1.)

    def _feat_hist_norm(self, feature):
        img = np.array(np.random.random((50, 50, 3)) * 255, dtype=np.uint8)
        self.assertAlmostEquals(np.sum(feature(cv.GetImage(cv.fromarray(img)))), 1.)

    def test_histogram_joint(self):
        self._histogram(imfeat.Histogram('rgb'))

    def test_histogram_joint_spatial(self):
        img = Image.open('test_images/lena.ppm')
        np.testing.assert_equal(imfeat.Histogram(mode='rgb')(img), imfeat.SpatialHistogram(mode='rgb')(img))
  
    def test_histogram_joint_lab(self):
        self._histogram(imfeat.Histogram('lab'))

    def test_meta(self):
        self._histogram(imfeat.MetaFeature(imfeat.Histogram('lab')))

    def test_moments(self):
        feature = imfeat.Moments('rgb', 2)
        for feat_out, image in self._run_all_images(feature):
            image = np.asfarray(image) / 255.
            image = image.reshape((image.shape[0] * image.shape[1], image.shape[2]))
            np.testing.assert_almost_equal(np.mean(image, 0), feat_out[0][:3])
            print(np.var(image, 0), feat_out[0][3:6])
            np.testing.assert_almost_equal(np.var(image, 0), feat_out[0][3:6])

    def test_rhog_gray(self):
        feature = imfeat.RHOG()
        for feat_out, image in self._run_all_images(feature):
            print(feat_out)
            print(len(feat_out[0]))

    def test_rhog_rgb(self):
        feature = imfeat.RHOG(gray=False)
        for feat_out, image in self._run_all_images(feature):
            print(feat_out)
            print(len(feat_out[0]))

    def test_gray_hist(self):
        feature = imfeat.Histogram('gray')
        for feat_out, image in self._run_all_images(feature):
            print(feat_out)
            print(len(feat_out[0]))
        self._feat_hist_zero(feature)
        self._feat_hist_norm(feature)

    def test_gradient_hist(self):
        feature = imfeat.GradientHistogram()
        for feat_out, image in self._run_all_images(feature):
            print(feat_out)
            print(len(feat_out[0]))
        self._feat_hist_zero(feature)
        self._feat_hist_norm(feature)

    def test_faces(self):
        feature = imfeat.Faces()
        for feat_out, image in self._run_all_images(feature):
            print(feat_out)
            print(len(feat_out[0]))

    def test_autocorrelogram(self):
        print('Autocorrelogram')
        feature = imfeat.Autocorrelogram()
        for feat_out, image in self._run_all_images(feature):
            print(feat_out)
            print(len(feat_out[0]))

    def test_hog_latent(self):
        print('Hog Latent')
        feature = imfeat.HOGLatent(2)
        for feat_out, image in self._run_all_images(feature):
            print(feat_out)
            print(len(feat_out[0]))
        print('Hog Latent')
        image = Image.open('test_images/lena.ppm')
        out = imfeat.compute(feature, image)[0]
        self.assertEqual(len(out), 254 * 254 * 32)
        np.testing.assert_equal(hashlib.md5(out.tostring()).hexdigest(), '3cc2af55af55bd429388d8be52fde356')
        load_from_umiacs('fixtures/lena_feat.pkl.gz', 'ab4580a8322e18b144c39867aeefa05b')
        with gzip.GzipFile('fixtures/lena_feat.pkl.gz') as fp:
            np.testing.assert_almost_equal(out, pickle.load(fp))

    def test_gist(self):
        feature = imfeat.GIST()
        for feat_out, image in self._run_all_images(feature):
            print(feat_out)
            print(len(feat_out[0]))
        # Compare against known output
        image = Image.open('test_images/lena.ppm')
        out = imfeat.compute(feature, image)[0]
        test_string = ' '.join(['%.4f' % x for x in out] + ['\n'])
        with open('fixtures/gist_lena_output.txt') as fp:
            true_string = fp.read()
        self.assertEqual(len(true_string.split()), len(test_string.split()))
        self.assertEqual(true_string, test_string)

            
if __name__ == '__main__':
    unittest.main()
