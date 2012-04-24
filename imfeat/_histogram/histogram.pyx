import cv
import numpy as np
cimport numpy as np
cimport imfeat

cdef extern from "histogram_aux.h":
    void histogram_joint_fast(float *data, int height, int width, float *min_vals, float *bin_width, np.int32_t *num_bins, np.int32_t *hist)
    void histogram_fast(float *data, int height, int width, float *min_vals, float *bin_width, np.int32_t *num_bins, np.int32_t *hist)
    void histogram_gray(float *data, int height, int width, float min_val, float bin_width, np.int32_t num_bins, np.int32_t *hist)

cdef class Histogram(imfeat.BaseFeature):
    cdef object mode
    cdef object verbose
    cdef np.ndarray min_vals
    cdef np.ndarray max_vals
    cdef np.ndarray num_bins
    cdef np.ndarray bin_width
    cdef int num_hist_bins
    cdef object style
    cdef object norm

    def __init__(self, mode, num_bins=8, style='joint', min_vals=None, max_vals=None, norm=True, verbose=False):
        super(Histogram, self).__init__({'type': 'numpy', 'dtype': 'float32', 'mode': mode})
        self.verbose = verbose
        self.norm = norm
        try:
            # From http://opencv.willowgarage.com/documentation/cpp/miscellaneous_image_transformations.html
            min_max_vals = dict([('bgr', ((0, 0, 0), (1, 1, 1))),
                                 ('rgb', ((0, 0, 0), (1, 1, 1))),
                                 ('xyz', ((0, 0, 0), (1, 1, 1))),
                                 ('ycrcb', ((0, 0, 0), (1, 1, 1))),
                                 ('hsv', ((0, 0, 0), (360, 1, 1))),
                                 ('luv', ((0, -134, -140), (100, 220, 122))),
                                 ('hls', ((0, 0, 0), (360, 1, 1))),
                                 ('lab', ((0, -127, -127), (100, 127, 127))),
                                 ('gray', ((0,), (1,)))])[mode]
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
        if style == 'joint':
            self.num_hist_bins = np.prod(self.num_bins)
        elif style == 'planar':
            self.num_hist_bins = np.sum(self.num_bins)
        else:
            raise ValueError('Style must be joint or planar!')
        self.style = style

    def __call__(self, image_np):
        image_np = self.convert(image_np)
        cdef np.ndarray image = image_np
        cdef np.ndarray out
        out = np.zeros(self.num_hist_bins, dtype=np.int32)
        height, width = image_np.shape[:2]
        if self.verbose:
            print('Min[%s] Max[%s]' % (np.min(np.min(image, 0), 0), np.max(np.max(image, 0), 0)))
        if self.mode == 'gray':
            histogram_gray(<float *>image.data, height, width, self.min_vals[0],
                           self.bin_width[0], self.num_bins[0], <np.int32_t *>out.data)
        else:
            if self.style == 'joint':
                histogram_joint_fast(<float *>image.data, height, width, <np.float32_t *>self.min_vals.data,
                                     <np.float32_t *>self.bin_width.data, <np.int32_t *>self.num_bins.data, <np.int32_t *>out.data)
            elif self.style == 'planar':
                histogram_fast(<float *>image.data, height, width, <np.float32_t *>self.min_vals.data,
                               <np.float32_t *>self.bin_width.data, <np.int32_t *>self.num_bins.data, <np.int32_t *>out.data)
            else:
                raise ValueError('Style must be joint or planar!')
        if self.norm:
            return np.asfarray(out) / np.sum(out)
        else:
            return np.asfarray(out)

