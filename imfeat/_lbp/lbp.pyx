import cv
import numpy as np
cimport numpy as np
cimport imfeat

cdef extern from "lbp_aux.h":
    void make_lbp_mask_fast(np.uint8_t *data, int height, int width, np.uint8_t *out)


cdef class LBP(imfeat.BaseFeature):
    cdef public object MODES

    def __init__(self, norm=True):
        super(LBP, self).__init__()
        self.MODES = [{'type': 'numpy', 'dtype': 'uint8', 'mode': 'gray'}]

    cpdef make_feature_mask(self, image_in):
        cdef np.ndarray[np.uint8_t, ndim=2, mode='c'] image = image_in
        cdef np.ndarray[np.uint8_t, ndim=2, mode='c'] out = np.zeros_like(image_in)
        make_lbp_mask_fast(<np.uint8_t *>image.data, image.shape[0], image.shape[1], <np.uint8_t *>out.data)
        return out

    cpdef make_features(self, image_in):
        pass
