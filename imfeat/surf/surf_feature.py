import ctypes
import numpy as np
import Image
from . import __path__

MODES = ['L']
MAX_PTS = 100

# Required Types
_char_ptr = ctypes.POINTER(ctypes.c_char)
_float_ptr = ctypes.POINTER(ctypes.c_float)
_int = ctypes.c_int32
# Load library
try:
    _surf = np.ctypeslib.load_library('libsurf_feature', __path__[0] + '/lib/')
except OSError:
    _surf = np.ctypeslib.load_library('libsurf_feature', '.')
_surf.compute_surf_descriptors.restype = ctypes.c_int
_surf.compute_surf_descriptors.argtypes = [_char_ptr, _int, _int, _int, _float_ptr]


def make_features(image):
    width, height = image.size
    points = np.ascontiguousarray(np.zeros((MAX_PTS, 64), dtype=np.float32))
    num_pts = _surf.compute_surf_descriptors(image.tostring(),
                                             height,
                                             width,
                                             MAX_PTS,
                                             points.ctypes.data_as(_float_ptr))
    return points[:num_pts]

if __name__ == "__main__":
    tests = ['real_test.jpg', 'real_test2.jpg']
    for test in tests:
        im = Image.open(test)
        st = time.time()
        print(make_features(im))
        print(time.time() - st)
