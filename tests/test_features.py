import unittest
import Image
import imfeat

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

    def test_histogram_joint(self):
        self._histogram(imfeat.Histogram('rgb'))
  

    def test_histogram_joint_lab(self):
        self._histogram(imfeat.Histogram('lab'))
            
if __name__ == '__main__':
    unittest.main()
