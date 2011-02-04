import histogram_joint
import numpy as np
import cv
import Image

MODES = ['RGB']

def make_features(image):
    # First convert to numpy
    rgbim = np.fromstring(image.tostring(),'u1')
    rgbim = rgbim.reshape((image.size[1],image.size[0],3))[:,:,::1]

    # Use OpenCV's functions to do a colorspace conversion
    cv.CvtColor(rgbim, rgbim, cv.CV_RGB2Lab)
    image_lab = Image.fromarray(rgbim)

    # Return the regular joint histogram
    return histogram_joint.make_features(image_lab)
