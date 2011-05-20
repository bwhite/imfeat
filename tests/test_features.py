import unittest
import Image
import imfeat
import numpy as np


class TestFeatures(unittest.TestCase):
    
    def setUp(self):
        self.image_names = ['test_images/test1.jpg', 
                            'test_images/test2.jpg']

    def _histogram(self, hf):
        # Test the histogram module by loading each test image
        # and converting it to each possible mode.
        # Also check that the size of the returned histogram is as
        # exepcted.
        for fn in self.image_names:
            img = Image.open(fn)
            h = imfeat.compute(hf, img)
            self.assertTrue(len(h) == 1)
            self.assertTrue(h[0].shape == (8*8*8,))

    def _run_all_images(self, feature):
        images = (Image.open(fn)
                  for fn in self.image_names)
        return ((imfeat.compute(feature, image), image)
                for image in images)

    def test_histogram_joint(self):
        self._histogram(imfeat.Histogram('rgb'))
  
    def test_histogram_joint_lab(self):
        self._histogram(imfeat.Histogram('lab'))

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

    def test_surf_random(self):
        feature = imfeat.SURF()
        for feat_out, image in self._run_all_images(feature):
            print(feat_out)
            print(len(feat_out[0]))

    def test_surf_detect(self):
        feature = imfeat.SURF(method='detect')
        for feat_out, image in self._run_all_images(feature):
            print(feat_out)
            print(len(feat_out[0]))

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
