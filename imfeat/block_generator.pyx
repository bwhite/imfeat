import numpy as np
cimport numpy as np
import cv
import imfeat

try:
    import cv2
except ImportError:
    print('OpenCV 2.3 needed for some functionality (you can ignore this but an error will be thrown if you use this functionality)')
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
        elif mode == 'similarity_cv2':
            tly, tlx, height, width, angle, scale = v
            cts = scale * np.cos(angle)
            sts = scale * np.sin(angle)
            trans = np.array([[cts, sts, -cts * tlx - sts * tly],
                              [-sts, cts, sts * tlx - cts * tly]])
            return (int(width), int(height)), trans

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


cdef class CoordGeneratorRectRotate(CoordGeneratorBase):
    cdef object _cgr
    cdef int _angle_steps
    cdef float _angle_delta
    cdef object _value
    cdef int _cur_angle_step
    
    def __init__(self, image_size, output_size, step_delta, angle_steps, *args, **kw):
        """
        Args:
            image_size: (height, width)
            output_size: (height, width)
            step_delta: (delta_y, delta_x)
            angle_steps: Number of steps for the angle
        """
        self._cgr = CoordGeneratorRect(image_size, output_size, step_delta)
        self._angle_steps = angle_steps
        self._angle_delta = 2 * np.pi / angle_steps
        self._value = None  # _value: (tly, tlx, height, width, angle, scale) of type
        self._cur_angle_step = 1  # This is 1 because we want to only perform _angle_steps rotations

    def __next__(self):
        if self._value is None or self._cur_angle_step >= self._angle_steps:
            self._value = self._cgr.next()
            self._cur_angle_step = 1
        else:
            self._value[4] += self._angle_delta
            self._cur_angle_step += 1
        return self._value.copy()


cdef class BlockGenerator(object):
    """Generates image blocks given a Coord Generator.  Ignores blocks that would be outside of the image."""

    cdef object _image
    cdef object _image_out
    cdef object _coord_gen

    def __init__(self, image, coord_gen_cls, *args, **kw):
        image = imfeat.convert_image(image, [('opencv', 'bgr', 8), ('opencv', 'gray', 8),
                                             ('opencv', 'bgr', 32), ('opencv', 'gray', 32)])
        assert(isinstance(image, cv.iplimage))
        self._image = image
        self._coord_gen = coord_gen_cls(image_size=(image.height, image.width), *args, **kw)
        self._image_out = cv.CreateImage((image.width, image.height), image.depth, image.channels)

    def __iter__(self):
        return self

    def __next__(self):
        while 1:
            sim = self._coord_gen.next()
            width_height, trans = self._coord_gen.format_output(sim, 'similarity_cv2')
            if (self._image_out.width, self._image_out.height) != width_height:
                self._image_out = cv.CreateImage(width_height, self._image.depth, self._image.channels)
            # See if coords are in bounds
            bounds = np.asfarray([[[0, 0, 1],
                                   [self._image_out.width, 0, 1],
                                   [0, self._image_out.height, 1],
                                   [self._image_out.width, self._image_out.height, 1]]])
            trans_inv = np.resize(trans, (3, 3))
            trans_inv[2, :] = np.array([0, 0, 1])
            bounds_trans = cv2.transform(bounds, np.linalg.inv(trans_inv))
            if np.any(bounds_trans < 0) or np.any(bounds_trans[0, :, 0] > self._image.width) or np.any(bounds_trans[0, :, 1] > self._image.height):
                continue
            cv.WarpAffine(self._image, self._image_out, trans)
            return self._image_out, sim
