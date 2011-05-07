import cv
import numpy as np
cimport numpy as np

cdef extern from "histogram_aux.h":
    void histogram_joint_fast(float *data, int height, int width, float *min_vals, float *bin_width, np.int32_t *num_bins, np.int32_t *hist)

cdef class Histogram(object):
    cdef public object MODES
    cdef object mode
    cdef np.ndarray min_vals
    cdef np.ndarray max_vals
    cdef np.ndarray num_bins
    cdef np.ndarray bin_width
    cdef int num_hist_bins

    def __init__(self, mode, min_vals, max_vals, num_bins, style='joint'):
        self.MODES = [('opencv', mode, 32)]
        self.mode = mode
        self.min_vals = np.ascontiguousarray(min_vals, dtype=np.float32)
        self.max_vals = np.ascontiguousarray(max_vals, dtype=np.float32)
        self.num_bins = np.ascontiguousarray(num_bins, dtype=np.int32)
        self.bin_width = np.ascontiguousarray((self.max_vals - self.min_vals) / self.num_bins, dtype=np.float32)
        self.num_hist_bins = np.prod(self.num_bins)

    cpdef make_features(self, image_cv):
        cdef np.ndarray out = np.zeros(self.num_hist_bins, dtype=np.int32)
        cdef np.ndarray image = np.ascontiguousarray(cv.GetMat(image_cv), dtype=np.float32)
        histogram_joint_fast(<float *>image.data, image_cv.height, image_cv.width, <np.float32_t *>self.min_vals.data,
                             <np.float32_t *>self.bin_width.data, <np.int32_t *>self.num_bins.data, <np.int32_t *>out.data)
        return [np.asfarray(out) / np.sum(out)]

