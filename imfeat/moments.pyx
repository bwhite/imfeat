import cv
import numpy as np
cimport numpy as np
cimport imfeat

cdef class Moments(imfeat.BaseFeature):
    cdef int _num_moments

    def __init__(self, mode, num_moments):
        """

        Args:
            mode: Open CV image mode (e.g., rgb, luv, hsv)
            num_moments: Moments to compute (2 implies mean + variance)
        """
        super(Moments, self).__init__({'type': 'numpy', 'dtype': 'float32', 'mode': mode})
        assert num_moments > 0
        self._num_moments = num_moments

    def __call__(self, image_np):
        image_np = self.convert(image_np)
        cdef np.ndarray image = np.asfarray(image_np, dtype=np.float64)
        cdef int num_pixels = image.shape[0] * image.shape[1]
        if image.ndim == 2:
            image = np.ascontiguousarray(image.reshape((num_pixels, 1)))
        else:
            image = np.ascontiguousarray(image.reshape((num_pixels, image.shape[2])))
        cdef np.ndarray mean = np.mean(image, 0)
        cdef np.ndarray diff = image - mean
        out = [mean]
        for m in range(2, self._num_moments + 1):
            out.append(np.sum((diff ** m), 0) / num_pixels)
        return np.hstack(out)
