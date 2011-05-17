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
            
            
if __name__ == '__main__':
    unittest.main()
