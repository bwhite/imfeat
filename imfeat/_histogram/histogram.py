import numpy as np
import ctypes
from . import __path__

MODES = ['RGB']

# Required Types
_char_ptr = ctypes.POINTER(ctypes.c_char)
_int_ptr = ctypes.POINTER(ctypes.c_int32)
_int = ctypes.c_int32
# Load library
try:
    _h = np.ctypeslib.load_library('libhistogram', __path__[0] + '/lib/')
except OSError:
    _h = np.ctypeslib.load_library('libhistogram', '.')
_h.compute.restype = ctypes.c_int
_h.compute.argtypes = [_int, _int, _int, _char_ptr, _int_ptr]


def _compute(num_channels, num_pixels, num_bins, data):
    bins = np.ascontiguousarray(np.zeros(num_bins * 3, dtype=np.int32))
    _h.compute(num_channels,
               num_pixels,
               num_bins,
               data,
               bins.ctypes.data_as(_int_ptr))
    return bins

def make_features(image):
    return [np.array(_compute(3, image.size[0] * image.size[1], 16, image.tostring()), dtype=np.float64)]
