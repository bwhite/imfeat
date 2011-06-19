import cv
import numpy as np
cimport numpy as np
cimport imfeat

cdef extern from "features.hpp":
    void process_feat_size(int height, int width, int sbin, np.int32_t *out)
    void process(np.float64_t *im_rowmajor, int height, int width, int sbin, np.float64_t *feat, int feat_size)
    void resize_size(int height, int width, int scale, np.int32_t *ddims)
    void resize(np.float64_t *im_rowmajor, int height, int width, double scale, np.float64_t *dst_rowmajor, int dst_rowmajor_size)

cdef class HOGLatent(imfeat.BaseFeature):
    cdef public object MODES
    cdef int _sbin

    def __init__(self, sbin=2):
        super(HOGLatent, self).__init__()
        self.MODES = [('opencv', 'rgb', 8)]
        self._sbin = sbin

    cpdef make_features(self, image_cv):
        cdef np.ndarray image = np.ascontiguousarray(cv.GetMat(image_cv), dtype=np.float64)
        cdef np.ndarray feat_shape = np.zeros(3, dtype=np.int32)
        process_feat_size(image_cv.height, image_cv.width, self._sbin, <np.int32_t *>feat_shape.data)
        cdef np.ndarray out = np.zeros(feat_shape[::-1], dtype=np.float64)
        process(<np.float64_t *>image.data, image_cv.height, image_cv.width, self._sbin, <np.float64_t *>out.data, np.prod(feat_shape))
        return [np.ascontiguousarray(out.T.ravel())]
