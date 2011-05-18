import cv
import numpy as np
cimport numpy as np

cdef extern from "rhog_aux.h":
    void compute_hog(np.uint8_t *image, int height, int width,
                     np.float64_t *block_bins, int cell_diameter,
                     int block_diameter, int orientation_bins)
    void compute_hog_rgb(np.uint8_t *imager, np.uint8_t *imageg,
                         np.uint8_t *imageb, int height, int width,
                         np.float64_t *block_bins, int cell_diameter,
                         int block_diameter, int orientation_bins)


cdef class RHOG(object):
    cdef public object MODES
    cdef int _cell_diameter
    cdef int _block_diameter
    cdef int _orientation_bins

    def __init__(self, cell_diameter=8, block_diameter=3, orientation_bins=9, gray=True):
        """

        Args:
            cell_diameter:
            block_diameter:
            orientation_bins:
        """
        # TODO Support color
        self._cell_diameter = cell_diameter
        self._block_diameter = block_diameter
        self._orientation_bins = orientation_bins
        self.MODES = [('opencv', 'gray' if gray else 'rgb', 8)]

    cpdef make_features(self, image_cv):
        cdef np.ndarray image = np.ascontiguousarray(cv.GetMat(image_cv), dtype=np.uint8)
        cdef np.ndarray image_r
        cdef np.ndarray image_g
        cdef np.ndarray image_b
        cdef int height = image.shape[0]
        cdef int width = image.shape[1] 
        celly = (height - 2) // self._cell_diameter
        cellx = (width - 2) // self._cell_diameter
        # For 1 cell stride
        nblock_bins = (celly - self._block_diameter + 1) * (cellx - self._block_diameter + 1) * self._block_diameter * self._block_diameter * self._orientation_bins
        # For 3 cell stride
        #nblock_bins = (celly // block_diameter) * (cellx // block_diameter) * orientation_bins * block_diameter * block_diameter
        cdef np.ndarray block_bins = np.zeros(nblock_bins, dtype=np.float64)
        if image.ndim == 2:
            compute_hog(<np.uint8_t *>image.data, height, width,
                        <double *>block_bins.data, self._cell_diameter,
                        self._block_diameter, self._orientation_bins)
        elif image.ndim == 3:
            image_r = np.ascontiguousarray(image[:, :, 0])
            image_g = np.ascontiguousarray(image[:, :, 1])
            image_b = np.ascontiguousarray(image[:, :, 2])
            compute_hog_rgb(<np.uint8_t *>image_r.data, <np.uint8_t *>image_g.data,
                            <np.uint8_t *>image_b.data, height, width,
                            <double *>block_bins.data, self._cell_diameter,
                            self._block_diameter, self._orientation_bins)
        else:
            raise ValueError('Image must be gray or rgb')
        return [block_bins]
