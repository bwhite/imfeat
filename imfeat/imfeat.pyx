import imfeat
import warnings


cdef class BaseFeature(object):

    def __init__(self):
        super(BaseFeature, self).__init__()

    def make_features(self, image):
        raise NotImplementedError

    def __call__(self, image):
        msg = ('This module uses the old style of feat.make_features, it '
               'should implement __call__ so that it can be used like '
               'feat(image).  It will have to call convert itself.')
        warnings.warn(msg, DeprecationWarning, stacklevel=2)
        with warnings.catch_warnings():
            return imfeat.compute(self, image)[0]
