import cv
import numpy as np
cimport numpy as np
cimport imfeat

cdef extern from "lbp_aux.h":
    void make_lbp_mask_fast(np.uint8_t *data, int height, int width, np.uint8_t *out)
    void make_lbp_mask_pool_fast(np.uint8_t *data, int height, int width, int radius, np.uint8_t *hist)


cdef class LBP(imfeat.BaseFeature):
    cdef public object MODES

    def __init__(self, norm=True):
        super(LBP, self).__init__()
        self.MODES = [{'type': 'numpy', 'dtype': 'uint8', 'mode': 'gray'}]

    cpdef make_feature_mask(self, image_in, pool_radius=0, num_best=1):
        cdef np.ndarray[np.uint8_t, ndim=2, mode='c'] image = image_in
        cdef np.ndarray[np.uint8_t, ndim=2, mode='c'] out = np.zeros_like(image_in)
        cdef np.ndarray[np.uint8_t, ndim=3, mode='c'] hist
        make_lbp_mask_fast(<np.uint8_t *>image.data, image.shape[0], image.shape[1], <np.uint8_t *>out.data)
        if pool_radius:
            hist = np.zeros((image_in.shape[0], image_in.shape[1], 256), dtype=np.uint8)
            make_lbp_mask_pool_fast(<np.uint8_t *>out.data, image.shape[0], image.shape[1], pool_radius, <np.uint8_t *>hist.data)
            print('MaxHist[%s]' % str(np.max(hist)))
            return np.array(np.argsort(hist, 2)[:, :, -num_best:], dtype=np.uint8)
        return out
