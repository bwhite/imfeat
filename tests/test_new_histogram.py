import imfeat
import cv
import numpy as np

img = cv.LoadImage('test_images/lena.jpg')
feat = imfeat.Histogram('bgr', [0, 0, 0], [1, 1, 1], [8, 8, 8])
a = imfeat.compute(imfeat.histogram_joint, img)
b = imfeat.compute(feat, img)
print(a[0])
print(b[0])
print(a[0] - b[0])
