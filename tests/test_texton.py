try:
    import unittest2 as unittest
except ImportError:
    import unittest
import imfeat
import impoint
import cv2

import numpy as np
import imseg
import os

msrc_classes = {'building': 9, 'sheep': 11, 'flower': 19, 'bicycle': 7, 'cow': 15, 'face': 13, 'sky': 8, 'tree': 1, 'dog': 4, 'sign': 0, 'water': 10, 'book': 20, 'body': 12, 'cat': 6, 'boat': 14, 'aeroplane': 17, 'car': 18, 'chair': 16, 'grass': 3, 'bird': 2, 'road': 5}
msrc_class_names = np.array([x[0] for x in sorted(msrc_classes.items(), key=lambda x: x[1])])

colors = np.asarray([[97, 37, 193], [35, 139, 35], [185, 199, 213], [20, 255, 100],
                     [19, 64, 139], [180, 180, 180], [165, 62, 163], [200, 214, 50],
                     [255, 20, 25], [201, 201, 205], [112, 25, 25], [99, 231, 9],
                     [131, 148, 32], [21, 183, 156], [201, 119, 122], [78, 63, 141],
                     [171, 7, 97], [43, 200, 126], [34, 189, 202], [207, 47, 46], [129, 176, 213]])  # NOTE (brandyn): BGR



def save_key():
    color_key = imseg.make_color_key(colors[:, ::-1], msrc_class_names)
    cv2.imwrite('texton_key.png', color_key)


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

        # Make a 4x4 texton image and replace the max labels with text
        img = imfeat.TextonImage()(image)
        print msrc_class_names[np.array(img, dtype=np.int)].reshape((4, 4))

        # Make label mask
        color_labels = colors[imfeat.TextonImage()._predict(image)[3]]
        cv2.imwrite('texton_labels.png', color_labels)

        # Make color key
        save_key()

if __name__ == '__main__':
    unittest.main()
