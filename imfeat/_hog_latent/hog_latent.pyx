
import cv
import numpy as np
cimport numpy as np
cimport imfeat
import imfeat

cdef extern from "features.hpp":
    void process_feat_size(int height, int width, int sbin, np.int32_t *out)
    void process(np.float64_t *im_rowmajor, int height, int width, int sbin, np.float64_t *feat, int feat_size)
    void resize_size(int height, int width, int scale, np.int32_t *ddims)
    void resize(np.float64_t *im_rowmajor, int height, int width, double scale, np.float64_t *dst_rowmajor, int dst_rowmajor_size)

cdef class HOGLatent(imfeat.BaseFeature):
    cdef int _sbin
    cdef object _blocks

    def __init__(self, sbin=2, blocks=1):
        super(HOGLatent, self).__init__({'type': 'numpy', 'dtype': 'uint8', 'mode': 'rgb'})
        self._sbin = sbin
        self._blocks = blocks

    def __reduce__(self):
        return (HOGLatent, (self._sbin, self._blocks))

    cpdef make_feature_mask(self, image_input):
        image_input = self.convert(image_input)
        cdef np.ndarray image = np.ascontiguousarray(image_input, dtype=np.float64)
        cdef np.ndarray feat_shape = np.zeros(3, dtype=np.int32)
        process_feat_size(image.shape[0], image.shape[1], self._sbin, <np.int32_t *>feat_shape.data)
        cdef np.ndarray out = np.zeros(feat_shape[::-1], dtype=np.float64)
        process(<np.float64_t *>image.data, image.shape[0], image.shape[1], self._sbin, <np.float64_t *>out.data, np.prod(feat_shape))
        out = out.T
        out = np.asfarray(out[:, :, :-1])
        xcoords, ycoords = np.meshgrid(range(image_input.shape[1]), range(image_input.shape[0]))
        xcoords = np.clip(xcoords / self._sbin, 0, out.shape[1] - 1)
        ycoords = np.clip(ycoords / self._sbin, 0, out.shape[0] - 1)
        return np.ascontiguousarray(out[ycoords, xcoords, :])

    cpdef compute_dense_2d(self, image_input, sbin=None, blocks=None):
        """Compute a set of dense feature vectors

        Intended for use where localization of the features is not necessary

        Args:
            image_input: Image input
            sbin: Bin size (default is to use what is used in the constructor)
            blocks: Number of cells per block (default is to use what is used in the constructor)

        Returns:
            Numpy array with dims (num_feat, num_dims)
        """
        image_input = self.convert(image_input)
        cdef np.ndarray image = np.ascontiguousarray(imfeat.convert_image(image_input, [{'type': 'numpy', 'mode': 'rgb', 'dtype': 'uint8'}]),
                                                     dtype=np.float64)
        cdef np.ndarray feat_shape = np.zeros(3, dtype=np.int32)
        if sbin is None:
            sbin = self._sbin
        if blocks is None:
            blocks = self._blocks
        sbin = int(sbin)
        blocks = int(blocks)
        process_feat_size(image.shape[0], image.shape[1], sbin, <np.int32_t *>feat_shape.data)
        cdef np.ndarray out = np.zeros(feat_shape[::-1], dtype=np.float64)
        process(<np.float64_t *>image.data, image.shape[0], image.shape[1], sbin, <np.float64_t *>out.data, np.prod(feat_shape))
        out = out.T
        out = np.asfarray(out[:, :, :-1])
        out = self._make_blocks(out, blocks)
        return np.ascontiguousarray(out)

    cpdef compute_dense(self, image_input, sbin=None, blocks=None):
        """Compute a set of dense feature vectors

        Intended for use where localization of the features is not necessary

        Args:
            image_input: Image input
            sbin: Bin size (default is to use what is used in the constructor)
            blocks: Number of cells per block (default is to use what is used in the constructor)

        Returns:
            Numpy array with dims (num_feat, num_dims)
        """
        out = self.compute_dense_2d(image_input, sbin=sbin, blocks=blocks)
        if out.ndims != 3:
            return np.array([], dtype=np.double)
        return np.ascontiguousarray(out.reshape((out.shape[0] * out.shape[1], out.shape[2])))

    def make_bow_mask(self, image_input, clusters, sbin=None, blocks=None):
        """Compute a 2D BoVW mask as a numpy array

        The resulting mask is not necessarily the same shape as the input
        but they are as they are produced by the feature.

        Args:
            image_input: Image input
            clusters: 2D numpy array (num_clusters, num_dims)
            sbin: Bin size (default is to use what is used in the constructor)
            blocks: Number of cells per block (default is to use what is used in the constructor)

        Returns:
            Numpy array with dims (rows, cols)
        """
        image_input = self.convert(image_input)
        if sbin is None:
            sbin = self._sbin
        if blocks is None:
            blocks = self._blocks
        sbin = int(sbin)
        blocks = int(blocks)
        import distpy
        dist = distpy.L2Sqr()
        cdef np.ndarray image = np.ascontiguousarray(imfeat.convert_image(image_input, [{'type': 'numpy', 'mode': 'rgb', 'dtype': 'uint8'}]),
                                                     dtype=np.float64)
        cdef np.ndarray feat_shape = np.zeros(3, dtype=np.int32)
        process_feat_size(image.shape[0], image.shape[1], sbin, <np.int32_t *>feat_shape.data)
        cdef np.ndarray out = np.zeros(feat_shape[::-1], dtype=np.float64)
        process(<np.float64_t *>image.data, image.shape[0], image.shape[1], sbin, <np.float64_t *>out.data, np.prod(feat_shape))
        out = out.T
        out = np.asfarray(out[:, :, :-1])
        out = self._make_blocks(out, blocks)
        bow_shape = out.shape[0], out.shape[1]
        out = out.reshape((out.shape[0] * out.shape[1], out.shape[2]))
        return np.ascontiguousarray(dist.nns(clusters, np.asfarray(out))[:, 1]).reshape(bow_shape)

    cdef _make_blocks(self, np.ndarray out, blocks):
        """Convert a grid of raw hog features into overlapping blocks"""
        cdef int x, y
        #if blocks == 1:  # Minor optimization
        #    return out
        return np.ascontiguousarray([[out[y:(y + blocks), x:(x + blocks), :].ravel() for x in range(out.shape[1] - blocks + 1)]
                                     for y in range(out.shape[0] - blocks + 1)])
        

    def __call__(self, image_input, ravel=True):
        image_input = self.convert(image_input)
        cdef np.ndarray image = np.ascontiguousarray(image_input, dtype=np.float64)
        cdef np.ndarray feat_shape = np.zeros(3, dtype=np.int32)
        process_feat_size(image.shape[0], image.shape[1], self._sbin, <np.int32_t *>feat_shape.data)
        cdef np.ndarray out = np.zeros(feat_shape[::-1], dtype=np.float64)
        process(<np.float64_t *>image.data, image.shape[0], image.shape[1], self._sbin, <np.float64_t *>out.data, np.prod(feat_shape))
        out = out.T
        out = np.asfarray(out[:, :, :-1])
        try:
            out = self._make_blocks(out, self._blocks)
        except ValueError:
            raise ValueError('Image is too small[%s]' % (image_input.shape,))
        if ravel:
            return np.ascontiguousarray(out.ravel())
        else:
            return np.ascontiguousarray(out)
