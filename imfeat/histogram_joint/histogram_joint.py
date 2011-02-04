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
    _hj = np.ctypeslib.load_library('libhistogram_joint', __path__[0] + '/lib/')
except OSError:
    _hj = np.ctypeslib.load_library('libhistogram_joint', '.')
_hj.compute.restype = ctypes.c_int
_hj.compute.argtypes = [_int, _int, _char_ptr, _int_ptr]


def _compute(num_pixels, num_bins, data):
    bins = np.ascontiguousarray(np.zeros(num_bins**3, dtype=np.int32))
    _hj.compute(num_pixels,
                num_bins,
                data,
                bins.ctypes.data_as(_int_ptr))
    bins = np.array(bins, dtype=np.float64)
    return bins / np.linalg.norm(bins, 1)

def make_features(image):
    assert image.mode in MODES
    return [np.array(_compute(image.size[0] * image.size[1], 16, image.tostring()), dtype=np.float64)]
