import numpy as np
import ctypes
from . import __path__

# Required Types
_char_ptr = ctypes.POINTER(ctypes.c_char)
_int_ptr = ctypes.POINTER(ctypes.c_int32)
_int = ctypes.c_int32
# Load library
try:
    _hj = np.ctypeslib.load_library('libhistogram_joint', __path__[0] + '/lib/')
except OSError:
    _hj = np.ctypeslib.load_library('libhistogram_joint', '.')
_hj.compute.restype = ctypes.c_int
_hj.compute.argtypes = [_int, _int, _char_ptr, _int_ptr]


def compute(num_pixels, num_bins, data):
    bins = np.ascontiguousarray(np.zeros(num_bins**3, dtype=np.int32))
    _hj.compute(num_pixels,
                num_bins,
                data,
                bins.ctypes.data_as(_int_ptr))
    return bins
