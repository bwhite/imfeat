import numpy as np
import ctypes
import Image

from . import __path__

MODES = ['L']

# Required Types
_char_ptr = ctypes.POINTER(ctypes.c_char)
_double_ptr = ctypes.POINTER(ctypes.c_double)
_int = ctypes.c_int32
# Load library
try:
    _shlib = np.ctypeslib.load_library('librhog', __path__[0] + '/lib/')
except OSError:
    _shlib = np.ctypeslib.load_library('librhog', '.')
_shlib.compute_hog.restype = ctypes.c_int
_shlib.compute_hog.argtypes = [_char_ptr, _int, _int, _double_ptr, _int, _int, _int]

def _compute(image, cell_diameter, block_diameter, orientation_bins):
    height = image.size[1] 
    width = image.size[0] 
    celly = (height - 2) // cell_diameter
    cellx = (width - 2) // cell_diameter
    # For 1 cell stride
    nblock_bins = (celly - block_diameter + 1) * (cellx - block_diameter + 1) * block_diameter * block_diameter * orientation_bins
    # For 3 cell stride
    #nblock_bins = (celly // block_diameter) * (cellx // block_diameter) * orientation_bins * block_diameter * block_diameter
    #print('cellx[%f] celly[%f] height[%f] width[%f] nblock_bins[%f]' % (cellx, celly, height, width, nblock_bins))
    block_bins = np.zeros(nblock_bins, dtype=np.float64)
    _shlib.compute_hog(image.tostring(),
                       height,
                       width,
                       block_bins.ctypes.data_as(_double_ptr),
                       cell_diameter,
                       block_diameter,
                       orientation_bins)
    return [block_bins]


def make_features(image, cell_diameter=8, block_diameter=3, orientation_bins=9):
    val = _compute(image, cell_diameter, block_diameter, orientation_bins)
    return val
