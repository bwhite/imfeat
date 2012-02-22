try:
    import unittest2 as unittest
except ImportError:
    import unittest
import imfeat
import impoint
import cv2

# Cheat Sheet (method/test) <http://docs.python.org/library/unittest.html>
#
# assertEqual(a, b)       a == b   
# assertNotEqual(a, b)    a != b    
# assertTrue(x)     bool(x) is True  
# assertFalse(x)    bool(x) is False  
# assertRaises(exc, fun, *args, **kwds) fun(*args, **kwds) raises exc
# assertAlmostEqual(a, b)  round(a-b, 7) == 0         
# assertNotAlmostEqual(a, b)          round(a-b, 7) != 0
# 
# Python 2.7+ (or using unittest2)
#
# assertIs(a, b)  a is b
# assertIsNot(a, b) a is not b
# assertIsNone(x)   x is None
# assertIsNotNone(x)  x is not None
# assertIn(a, b)      a in b
# assertNotIn(a, b)   a not in b
# assertIsInstance(a, b)    isinstance(a, b)
# assertNotIsInstance(a, b) not isinstance(a, b)
# assertRaisesRegexp(exc, re, fun, *args, **kwds) fun(*args, **kwds) raises exc and the message matches re
# assertGreater(a, b)       a > b
# assertGreaterEqual(a, b)  a >= b
# assertLess(a, b)      a < b
# assertLessEqual(a, b) a <= b
# assertRegexpMatches(s, re) regex.search(s)
# assertNotRegexpMatches(s, re)  not regex.search(s)
# assertItemsEqual(a, b)    sorted(a) == sorted(b) and works with unhashable objs
# assertDictContainsSubset(a, b)      all the key/value pairs in a exist in b
import numpy as np

msrc_classes = {'building': 9, 'sheep': 11, 'flower': 19, 'bicycle': 7, 'cow': 15, 'face': 13, 'sky': 8, 'tree': 1, 'dog': 4, 'sign': 0, 'water': 10, 'book': 20, 'body': 12, 'cat': 6, 'boat': 14, 'aeroplane': 17, 'car': 18, 'chair': 16, 'grass': 3, 'bird': 2, 'road': 5}
msrc_classes = np.array([x[0] for x in sorted(msrc_classes.items(), key=lambda x: x[1])])

class Test(unittest.TestCase):

    def setUp(self):
        pass

    def tearDown(self):
        pass

    def test_name(self):
        fn = 'test_images/texton.jpg'
        image = cv2.imread(fn)
        out = imfeat.TextonHistogram()(image)
        print out
        print out.shape
        out = imfeat.TextonSpatialHistogram()(image)
        print out
        print out.shape
        out = imfeat.TextonSpatialHistogram(levels=2)(image)
        print out
        print out.shape
        img = imfeat.TextonImage()(image)
        print msrc_classes[np.array(img, dtype=np.int)].reshape((4, 4))

if __name__ == '__main__':
    unittest.main()
