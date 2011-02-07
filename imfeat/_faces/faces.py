import cv
import numpy as np
from . import __path__

MODES = ['L', 'RGB']
_cascade = None


def _setup():
    global _cascade
    try:
        cascade_path = ('haarcascade_frontalface_default.xml')
        _cascade = cv.Load(cascade_path)
    except:
        cascade_path = (__path__[0] + '/data/haarcascade_frontalface_default.xml')
        _cascade = cv.Load(cascade_path)


def _detect_faces(img, cascade):
    min_size = (20, 20)
    image_scale = 2
    haar_scale = 1.2
    min_neighbors = 2
    haar_flags = 0
    if img.nChannels == 3:
        gray = cv.CreateImage((img.width, img.height), 8, 1)
        cv.CvtColor(img, gray, cv.CV_BGR2GRAY)
    else:
        gray = img
    small_img = cv.CreateImage((cv.Round(img.width / image_scale),
                                cv.Round(img.height / image_scale)), 8, 1)
    cv.Resize(gray, small_img, cv.CV_INTER_LINEAR)
    cv.EqualizeHist(small_img, small_img)
    faces = cv.HaarDetectObjects(small_img, cascade, cv.CreateMemStorage(0),
                                 haar_scale, min_neighbors, haar_flags,
                                 min_size)
    return [((x * image_scale, y * image_scale,
              w * image_scale, h * image_scale), n)
            for (x, y, w, h), n in faces]


def _demo(image):
    if not _cascade:
        _setup()
    faces = _detect_faces(image, _cascade)
    if faces:
        for ((x, y, w, h), n) in faces:
            pt1 = (int(x), int(y))
            pt2 = (int((x + w)), int((y + h)))
            cv.Rectangle(image, pt1, pt2, cv.RGB(255, 0, 0), 3, 8, 0)
    cv.ShowImage('Faces', image)
    while cv.WaitKey(10) == -1:
        pass


def _pil_to_cv(image):
    if image.mode == 'RGB':
        cv_image = cv.CreateImageHeader(image.size, cv.IPL_DEPTH_8U, 3)
    else:
        cv_image = cv.CreateImageHeader(image.size, cv.IPL_DEPTH_8U, 1)
    cv.SetData(cv_image, image.tostring())
    return cv_image


def make_features(image):
    if not _cascade:
        _setup()
    faces = _detect_faces(_pil_to_cv(image), _cascade)
    if faces:
        mean_pos = np.mean(np.asfarray([x[0] for x in faces]), 0)
        mean_pos[0] /= image.size[0]
        mean_pos[1] /= image.size[1]
        return [np.asfarray([len(faces)] + mean_pos.tolist())]
    return [np.zeros(5)]


if __name__ == '__main__':
    _demo(cv.LoadImage('../tests/lena.jpg'))
