import numpy as np
import ctypes
import Image

from . import __path__

MODES = ['RGB']

# Required Types
_char_ptr = ctypes.POINTER(ctypes.c_char)
_double_ptr = ctypes.POINTER(ctypes.c_double)
_int = ctypes.c_int32
# Load library
try:
    _moments = np.ctypeslib.load_library('libmoments', __path__[0] + '/lib/')
except OSError:
    _moments = np.ctypeslib.load_library('libmoments', '.')
_moments.compute.restype = ctypes.c_int
_moments.compute.argtypes = [_int, _int, _int, _char_ptr, _double_ptr]

def _compute(num_channels, num_pixels, num_moments, data):
    moments = np.zeros(num_moments * num_channels, dtype=np.float64)
    _moments.compute(num_channels,
                     num_pixels,
                     num_moments,
                     data,
                     moments.ctypes.data_as(_double_ptr))
    return moments


def make_features(image, num_moments=2):
    return [_compute(3,
                     image.size[0] * image.size[1],
                     num_moments,
                     image.tostring())]
