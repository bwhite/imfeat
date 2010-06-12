import numpy as np
import ctypes
from . import __path__

# Required Types
_char_ptr = ctypes.POINTER(ctypes.c_char)
_float_ptr = ctypes.POINTER(ctypes.c_float)
_int = ctypes.c_int32
# Load library
try:
    _moments = np.ctypeslib.load_library('libmoments', __path__[0] + '/lib/')
except OSError:
    _moments = np.ctypeslib.load_library('libmoments', '.')
_moments.compute.restype = ctypes.c_int
_moments.compute.argtypes = [_int, _int, _int, _char_ptr, _float_ptr]

def compute(num_channels, num_pixels, num_moments, data):
    moments = np.zeros(num_moments * num_channels, dtype=np.float32)
    _moments.compute(num_channels,
                     num_pixels,
                     num_moments,
                     data,
                     moments.ctypes.data_as(_float_ptr))
    return moments

