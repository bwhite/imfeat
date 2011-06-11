import numpy as np
cimport numpy as np
import cv
import imfeat

cdef class CoordGeneratorBase(object):
    cdef object _mode
    cdef object _sim_tuple

    def __init__(self, mode='affine_numpy'):
        # Modes: 'similarity_numpy', 'euclidean_pil', 'similarity_cv', 'similarity_namedtuple'
        self._mode = mode
        if mode == 'similarity_namedtuple':
            import collections
            self._sim_tuple = collections.namedtuple('SimilarityTuple', ['tly', 'tlx',
                                                                         'height', 'width',
                                                                         'angle', 'scale'])

    cpdef format_output(self, np.ndarray[np.float64_t, ndim=1, mode='c'] v, mode=None):
        """Format the output as specified by self._mode

        This provides convenient output formats for various purposes while
        abstracting this burden away from window generators.

        Args:
            v: (tly, tlx, height, width, angle, scale) of type
                ndarray[np.float64_t, ndim=1, mode='c']

        Returns:
            Value in the form specified in self._mode
        """
        if not mode:
            mode = self._mode
        if mode == 'affine_numpy':
            return v
        elif mode == 'similarity_namedtuple':
            return self._sim_tuple(v[0], v[1], v[2], v[3], v[4], v[5])
        elif mode == 'similarity_cv':
            tly, tlx, height, width, angle, scale = v
            cts = scale * np.cos(angle)
            sts = scale * np.sin(angle)
            trans = np.array([[cts, sts, -cts * tlx - sts * tly],
                              [-sts, cts, sts * tlx - cts * tly]])
            trans_cv = cv.CreateMatHeader(2, 3, cv.CV_64FC1)
            cv.SetData(trans_cv, trans.tostring())
            return (int(width), int(height)), trans_cv
        elif self._mode == 'euclidean_pil':
            # (left, upper, right, lower) pixel coords compatible with
            # PIL's Image.crop.  x is [left, right) and y is [upper, lower)
            assert angle == 0. and scale == 1.
            upper, left = v[:2] - v[2:4] / 2
            lower, right = v[:2] + v[2:4] / 2
            return left, upper, right, lower
        else:
            raise ValueError('Invalid mode')

    def __iter__(self):
        return self

    def __next__(self):
        raise NotImplementedError('You must provide this method')

cdef class CoordGeneratorRect(CoordGeneratorBase):
    cdef np.ndarray _image_size
    cdef np.ndarray _output_size
    cdef np.ndarray _output_size_half
    cdef np.ndarray _step_delta
    cdef np.ndarray _initial_pos
    cdef np.ndarray _next_pos
    cdef np.ndarray _output

    def __init__(self, image_size, output_size, step_delta, *args, **kw):
        """
        Args:
            image_size: (height, width)
            output_size: (height, width)
            step: (delta_y, delta_x)
        """
        super(CoordGeneratorRect, self).__init__(*args, **kw)
        self._image_size = np.ascontiguousarray(image_size, dtype=np.int32)
        self._output_size = np.ascontiguousarray(output_size, dtype=np.int32)
        self._output_size_half = self._output_size / 2.        
        self._step_delta = np.ascontiguousarray(step_delta, dtype=np.int32)
        np.testing.assert_array_less([0, 0], image_size)
        np.testing.assert_array_less([0, 0], output_size)
        np.testing.assert_array_less([0, 0], step_delta)
        self._initial_pos = (self._image_size % self._step_delta) / 2 # self._output_size_half + 
        self._next_pos = self._initial_pos.copy()
        self._output = np.zeros(6, dtype=np.float64)
        self._output[2:4] = self._output_size
        self._output[5] = 1.

    def __next__(self):
        if self._next_pos[1] + self._output_size_half[1] <= self._image_size[1]:
            self._output[:2] = self._next_pos
            self._next_pos[1] += self._step_delta[1]
            return self.format_output(self._output.copy())
        self._next_pos[0] += self._step_delta[0]
        if self._next_pos[0] + self._output_size_half[0] <= self._image_size[0]:
            self._next_pos[1] = self._initial_pos[1]
            return self.format_output(self.next())
        raise StopIteration


cdef class BlockGenerator(object):

    cdef object _image
    cdef object _image_out
    cdef object _coord_gen

    def __init__(self, image, coord_gen_cls, *args, **kw):
        assert(isinstance(image, cv.iplimage))
        self._image = image
        self._coord_gen = coord_gen_cls(image_size=(image.height, image.width), *args, **kw)
        self._image_out = cv.CreateImage((image.width, image.height), image.depth, image.channels)

    def __iter__(self):
        return self

    def __next__(self):
        sim = self._coord_gen.next()
        width_height, trans = self._coord_gen.format_output(sim, 'similarity_cv')
        if (self._image_out.width, self._image_out.height) != width_height:
            self._image_out = cv.CreateImage(width_height, self._image.depth, self._image.channels)
        cv.WarpAffine(self._image, self._image_out, trans)
        return self._image_out, sim
