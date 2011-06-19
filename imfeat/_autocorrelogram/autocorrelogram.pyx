import cv
import numpy as np
cimport numpy as np
cimport imfeat

cdef extern from "Autocorrelogram.h":
    void autocorrelogram(np.uint8_t *data, int height, int width, int unique_colors,
                         int *distance_set, int distance_set_size, np.float64_t *ac_out, int ac_out_size)
    void convert_colors_rg16(np.uint8_t *data, int size, np.uint8_t *out)
    void convert_colors_rg64(np.uint8_t *data, int size, np.uint8_t *out)

cdef class Autocorrelogram(imfeat.BaseFeature):
    cdef public object MODES
    cdef np.ndarray distance_set
    cdef int unique_colors

    def __init__(self, distance_set=(1,3,5,7), unique_colors=64):
        super(Autocorrelogram, self).__init__()
        assert unique_colors == 16 or unique_colors == 64
        self.MODES = [('opencv', 'rgb', 8)]
        self.distance_set = np.ascontiguousarray(distance_set, dtype=np.int32)
        self.unique_colors = unique_colors

    cpdef make_features(self, image_cv):
        cdef np.ndarray image = np.ascontiguousarray(cv.GetMat(image_cv), dtype=np.uint8)
        cdef int out_ac_size = self.unique_colors * len(self.distance_set)
        cdef np.ndarray out = np.zeros(out_ac_size)
        cdef np.ndarray image_packed = np.zeros(image_cv.height * image_cv.width, dtype=np.uint8)
        if self.unique_colors == 64:
            convert_colors_rg64(<np.uint8_t *>image.data, image_cv.height * image_cv.width, <np.uint8_t *>image_packed.data)
        elif self.unique_colors == 16:
            convert_colors_rg16(<np.uint8_t *>image.data, image_cv.height * image_cv.width, <np.uint8_t *>image_packed.data)
        else:
            raise ValueError('Invalid unique_colors value!')
        autocorrelogram(<np.uint8_t *>image_packed.data, image_cv.height, image_cv.width, self.unique_colors,
                        <int *>self.distance_set.data, self.distance_set.size, <np.float64_t *>out.data, out_ac_size)
        return [np.nan_to_num(out)]
