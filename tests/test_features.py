import unittest
import Image

class TestFeatures(unittest.TestCase):
    
    def setUp(self):
        self.image_names = ['test1.jpg', 
                            'test2.jpg',
                            'test3.gif',
                            'test4.pgm']

    def histogram_test(self, hf):
        # Test the histogram module by loading each test image
        # and converting it to each possible mode.
        # Also check that the size of the returned histogram is as
        # exepcted.

        for fn in self.image_names:
            img = Image.open(fn)
            for mode in hf.MODES:
                h = hf.make_features(img.convert(mode))
                self.assertTrue(len(h) == 1)
                self.assertTrue(h[0].shape == (16*16*16,))

    def test_histogram_joint(self):
        from imfeat import histogram_joint
        self.histogram_test(histogram_joint)
  

    def test_histogram_joint_lab(self):
        from imfeat import histogram_joint_lab
        self.histogram_test(histogram_joint_lab)
            
if __name__ == '__main__':
    unittest.main()
