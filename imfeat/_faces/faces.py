import cv
import numpy as np
import os
import imfeat
from . import __path__


def _detect_faces(img, cascade):
    min_size = (20, 20)
    image_scale = 2
    haar_scale = 1.2
    min_neighbors = 2
    haar_flags = 0
    small_img = cv.CreateImage((cv.Round(img.width / image_scale),
                                cv.Round(img.height / image_scale)), 8, 1)
    cv.Resize(img, small_img, cv.CV_INTER_LINEAR)
    cv.EqualizeHist(small_img, small_img)
    faces = cv.HaarDetectObjects(small_img, cascade, cv.CreateMemStorage(0),
                                 haar_scale, min_neighbors, haar_flags,
                                 min_size)
    return [((x * image_scale, y * image_scale,
              w * image_scale, h * image_scale), n)
            for (x, y, w, h), n in faces]


class Faces(imfeat.BaseFeature):

    def __init__(self):
        super(Faces, self).__init__()
        self.MODES = [('opencv', 'gray', 8)]
        path = 'haarcascade_frontalface_default.xml'
        if os.path.exists(path):
            self._cascade = cv.Load(path)
        else:
            path = (__path__[0] + '/data/haarcascade_frontalface_default.xml')
            if os.path.exists(path):
                self._cascade = cv.Load(path)
            else:
                raise ValueError("Can't find .xml file!")

    def make_features(self, image):
        faces = _detect_faces(image, self._cascade)
        if faces:
            mean_pos = np.mean(np.asfarray([x[0] for x in faces]), 0)
            mean_pos[0] /= image.size[0]
            mean_pos[1] /= image.size[1]
            return [np.asfarray([len(faces)] + mean_pos.tolist())]
        return [np.zeros(5)]
