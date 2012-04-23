import cv
import numpy as np
cimport numpy as np
cimport imfeat

cdef extern from "gist_wrapper.h":
    void color_gist_scaletab_wrap(np.uint8_t *data, int height, int width, int nblocks, int n_scale,
                                  np.int32_t *orientations_per_scale, np.float32_t *desc, int desc_size)

cdef class GIST(imfeat.BaseFeature):
    cdef public object MODES
    cdef int _nblocks
    cdef np.ndarray _orientations_per_scale

    def __init__(self, nblocks=4, orientations_per_scale=(8, 8, 4)):
        super(GIST, self).__init__({'type': 'numpy', 'dtype': 'uint8', 'mode': 'rgb'})
        self._nblocks = nblocks
        self._orientations_per_scale = np.array(orientations_per_scale, dtype=np.int32)

    cdef int _color_gist_scaletab_descsize(self):
        descsize = 0
        for orientations in self._orientations_per_scale:
            descsize += self._nblocks * self._nblocks * orientations
        return descsize * 3

    def __call__(self, image_np):
        cdef np.ndarray image = self.convert(image_np)
        descsize = self._color_gist_scaletab_descsize()
        cdef np.ndarray out = np.zeros(descsize, dtype=np.float32)
        color_gist_scaletab_wrap(<np.uint8_t *>image.data, image.shape[0], image.shape[1], self._nblocks,
                                 len(self._orientations_per_scale),
                                 <np.int32_t *>self._orientations_per_scale.data, <np.float32_t *>out.data, descsize)
        return np.asfarray(out)
