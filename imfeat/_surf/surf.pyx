import cv
import numpy as np
cimport numpy as np

cdef extern from "surf_feature.hpp":
    int compute_surf_descriptors(np.uint8_t *data, int height, int width, int max_points, float *points)
    int compute_surf_points(np.uint8_t *data, int height, int width, int max_points, np.float32_t *points,
                            np.int32_t *x, np.int32_t *y, np.int32_t *scale, np.float32_t *orientation,
                            np.uint8_t *sign, np.float32_t *cornerness)
    int compute_surf_random(np.uint8_t *data, int height, int width, int max_points, np.float32_t *points)

cdef class SURF(object):
    cdef public object MODES
    cdef object _method
    cdef int _max_points

    def __init__(self, method='random', max_points=10000):
        self.MODES = [('opencv', 'gray', 8)]
        self._method = method
        self._max_points = max_points

    cpdef make_features(self, image_cv):
        cdef np.ndarray image = np.ascontiguousarray(cv.GetMat(image_cv), dtype=np.uint8)
        cdef int height = image.shape[0]
        cdef int width = image.shape[1]
        max_points = min(self._max_points, height * width)
        cdef np.ndarray points = np.ascontiguousarray(np.zeros((max_points, 64), dtype=np.float32))
        if self._method == 'random':
            num_pts = compute_surf_random(<np.uint8_t *>image.data,
                                          height,
                                          width,
                                          max_points,
                                          <np.float32_t *>points.data)
        elif self._method == 'detect':
            num_pts = compute_surf_descriptors(<np.uint8_t *>image.data,
                                          height,
                                          width,
                                          max_points,
                                          <np.float32_t *>points.data)
        else:
            raise ValueError('Method not supported')
        return points[:num_pts]
