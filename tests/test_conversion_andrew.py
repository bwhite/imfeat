#!/usr/bin/env python
# (C) Copyright 2010 Dapper Vision Inc
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
"""Tests for image conversions between various IPL and OpenCV formats
"""
__author__ = 'Andrew Miller <amiller@dappervision.com>'
__license__ = 'Apache'

import unittest
import Image
import cv
import imfeat
import numpy as np

# This is a generous threshold used to check that RGB/Gray conversion
# is consistent
PIXEL_DIFF_THRESHOLD = 5

# The output modes to test
modes = ["RGB",
         ('opencv', 'bgr', cv.IPL_DEPTH_8U),
         ('opencv', 'gray', cv.IPL_DEPTH_8U),
         'L']

image_names = ["lena.jpg",
               "test1.jpg",
               "test2.jpg",               
               "test3.gif",
               "test4.pgm",
               "animated.gif",
               "one_pixel.gif",
               "dice_rgba.png",
               "monochrome_animated",
               "lena.ppm"]

image_names = ['test_images/' + name for name in image_names]


def pilRGBtoarray(image):
    assert isinstance(image, Image.Image)
    assert image.mode == 'RGB'
    return np.fromstring(image.tostring(), 'u1').reshape(image.size[0],
                                                         image.size[1],
                                                         3)
def cvBGRtoarray(image):
    assert isinstance(image, cv.iplimage)
    assert image.channels == 3
    assert image.depth == cv.IPL_DEPTH_8U
    return np.fromstring(image.tostring(), 'u1').reshape(image.width,
                                                         image.height,
                                                         3)[:,:,::-1]

def cvGRAYtoarray(image):
    assert isinstance(image, cv.iplimage)
    assert image.channels == 1
    assert image.depth == cv.IPL_DEPTH_8U
    return np.fromstring(image.tostring(), 'u1').reshape(image.width,
                                                         image.height)


def coerce_rgbarray(image):
    """Converts a PIL or OpenCV image to a 3 channel numpy array.
    Gray to RGB is performed by simply stacking the image 3 times

    Args:
        image: PIL image (L or RGB) or OpenCV image (gray or BGR), 8-bit

    Returns:
        numpy array, type 'u1', shape (w,h,3) where (w,h) is the input size
    """

    if isinstance(image, Image.Image):
        return pilRGBtoarray(image.convert("RGB"))
    elif isinstance(image, cv.iplimage):
        assert image.depth == cv.IPL_DEPTH_8U
        if image.channels == 1:
            return np.dstack(3*[cvGRAYtoarray(image)])
        else:
            return cvBGRtoarray(image)
            

class TestConversion(unittest.TestCase):
    def _assert_close_rgb(self, actual, desired):
        actual = coerce_rgbarray(actual)
        desired = coerce_rgbarray(desired)

        # Check that reversing the order doesn't improve the score
        self.assertFalse(np.abs(actual.astype('f')-desired[:,:,::-1]).max() < \
                         np.abs(actual.astype('f')-desired).max(),
                         "BGR vs RGB order is probably wrong")

        # Check that the two images agree
        self.assertTrue(np.all(np.abs(actual.astype('f') - \
                                      desired)<PIXEL_DIFF_THRESHOLD),
                        "Images differ by more than %d for at least one pixel" % PIXEL_DIFF_THRESHOLD)

    def _test_convert(self, rgb_func, gray_func, is_rgb):
        for image_name in image_names:
            try:
                image = rgb_func(image_name)
                image_gray = gray_func(image_name)
            except IOError:
                # OpenCV doesn't like reading in animated gifs,
                # or pallete gifs
                continue
            for mode in modes:
                print((image_name, mode))
                try:
                    # Explicitly ignore the case of converting Gray to RGB
                    if not is_rgb and not (mode == 'L' or 'gray' in mode):
                        continue
                    
                    # Use either the gray or the RGB version as input based
                    # on the parameter
                    if is_rgb:
                        actual = imfeat.convert_image(image, [mode])
                    else:
                        actual = imfeat.convert_image(image_gray, [mode])

                    # Compare against the gray or RGB input depending on
                    # the output mode
                    if mode == 'L' or 'gray' in mode:
                        self._assert_close_rgb(actual, image_gray)
                    else:
                        self._assert_close_rgb(actual, image)
                except:
                    print "Failed case: (convert to %s) %s [input %s]" % \
                          (mode, image_name, 'rgb' if is_rgb else 'gray')
                    raise


    def test_cvhsv(self):
        """Check that HSV conversion produces correct values for a solid color
        test case
        """
        
        # Check that HSV produces a 'blue' hue for a blue image
        hsv_target = 120.0

        # Build the image in BGR order
        blue_image = np.tile([255,0,0],(10,10,1)).astype('u1')
        image = cv.CreateImageHeader((10,10), cv.IPL_DEPTH_8U,
                                     3)
        cv.SetData(image, blue_image.tostring())
        mode = ('opencv', 'hsv', cv.IPL_DEPTH_8U)
        hsv = imfeat.convert_image(image, [mode])
        hsv = cvBGRtoarray(hsv)
        np.testing.assert_allclose(hsv[:,:,2], hsv_target)


    def test_transparent_png(self):
        """Transparent PNGs read in by PIL (mode RGBA) aren't supported
        """
        image = Image.open('test_images/dice_rgba.png')
        assert image.mode == "RGBA"
        mode = ('opencv', 'bgr', cv.IPL_DEPTH_8U)        
        actual = imfeat.convert_image(image, [mode])
        self._assert_close_rgb(actual, image)


    def test_cvbgr(self):
        """Exhaustively test all input images as read by opencv in BGR mode,
        and all output modes
        """
        bgr_func = lambda image_name: cv.LoadImage(image_name, iscolor=1)
        depth_func = lambda image_name: cv.LoadImage(image_name, iscolor=0)
        self._test_convert(bgr_func, depth_func, True)


    def test_cvgray(self):
        """Exhaustively test all input images as read by opencv in gray mode,
        and all output modes
        """
        rgb_func = lambda image_name: cv.LoadImage(image_name, iscolor=1)
        depth_func = lambda image_name: cv.LoadImage(image_name, iscolor=0)
        self._test_convert(rgb_func, depth_func, False)


    def test_pilrgb(self):
        """Exhaustively test all input images as read by PIL (converted RGB)
        and all output modes
        """
        rgb_func = lambda image_name: Image.open(image_name).convert("RGB")
        depth_func = lambda image_name: Image.open(image_name).convert("L")
        self._test_convert(rgb_func, depth_func, True)


    def test_pilgray(self):
        """Exhaustively test all input images as read by PIL (converted gray)
        and all output modes
        """
        rgb_func = lambda image_name: Image.open(image_name).convert("RGB")
        depth_func = lambda image_name: Image.open(image_name).convert("L")
        self._test_convert(rgb_func, depth_func, False)

            
if __name__ == '__main__':
    unittest.main()
