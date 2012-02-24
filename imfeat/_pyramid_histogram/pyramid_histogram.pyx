import cv
import numpy as np
cimport numpy as np
import imfeat
cimport imfeat

cdef extern from "pyramid_histogram_aux.h":
    void image_to_bin_map(float *data, int height, int width, float *min_vals, float *bin_width, np.int32_t *num_bins, np.int32_t *bin_map)

cdef class PyramidHistogram(imfeat.BaseFeature):
    cdef public object MODES
    cdef object mode
    cdef np.ndarray min_vals
    cdef np.ndarray max_vals
    cdef np.ndarray num_bins
    cdef np.ndarray bin_width
    cdef int num_bin_vals
    cdef int levels
    cdef object norm

    def __init__(self, mode, num_bins=8, min_vals=None, max_vals=None, norm=True, levels=1):
        super(PyramidHistogram, self).__init__()
        self.MODES = [{'type': 'numpy', 'mode': mode, 'dtype': 'float32'}]
        self.norm = norm
        self.levels = levels
        try:
            # From http://opencv.willowgarage.com/documentation/cpp/miscellaneous_image_transformations.html
            min_max_vals = dict([('bgr', ((0, 0, 0), (1, 1, 1))),
                                 ('rgb', ((0, 0, 0), (1, 1, 1))),
                                 ('xyz', ((0, 0, 0), (1, 1, 1))),
                                 ('ycrcb', ((0, 0, 0), (1, 1, 1))),
                                 ('hsv', ((0, 0, 0), (360, 1, 1))),
                                 ('luv', ((0, -134, -140), (100, 220, 122))),
                                 ('hls', ((0, 0, 0), (360, 1, 1))),
                                 ('lab', ((0, -127, -127), (100, 127, 127)))])[mode]
        except KeyError:
            raise ValueError('Mode not implemented [%s]' % mode)
        if isinstance(num_bins, (int, float)):
            num_bins = [num_bins] * len(min_max_vals[0])
        self.mode = mode
        if min_vals:
            self.min_vals = np.ascontiguousarray(min_vals, dtype=np.float32)
        else:
            self.min_vals = np.ascontiguousarray(min_max_vals[0], dtype=np.float32)
        if max_vals:
            self.max_vals = np.ascontiguousarray(max_vals, dtype=np.float32)
        else:
            self.max_vals = np.ascontiguousarray(min_max_vals[1], dtype=np.float32)
        self.num_bins = np.ascontiguousarray(num_bins, dtype=np.int32)
        self.bin_width = (self.max_vals - self.min_vals) / self.num_bins
        self.bin_width = np.ascontiguousarray(self.bin_width, dtype=np.float32)
        self.num_bin_vals = np.prod(self.num_bins)

    cpdef make_features(self, np.ndarray[np.float32_t, ndim=3, mode='c'] image):
        cdef np.ndarray bin_map = np.zeros((image.shape[0], image.shape[1]), dtype=np.int32)
        # Compute the bin map
        image_to_bin_map(<float *>image.data, image.shape[0], image.shape[1], <np.float32_t *>self.min_vals.data,
                         <np.float32_t *>self.bin_width.data, <np.int32_t *>self.num_bins.data, <np.int32_t *>bin_map.data)
        out = imfeat.spatial_pyramid(bin_map, self.levels, self.num_bin_vals)
        out = np.asfarray(out)
        if self.norm:
            out /= np.sum(out)
        return [out]
