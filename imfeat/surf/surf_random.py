import ctypes
import numpy as np
import Image
from . import __path__

MODES = ['L']

# Required Types
_char_ptr = ctypes.POINTER(ctypes.c_char)
_float_ptr = ctypes.POINTER(ctypes.c_float)
_int_ptr = ctypes.POINTER(ctypes.c_int32)
_bool_ptr = ctypes.POINTER(ctypes.c_bool)
_int = ctypes.c_int32
# Load library
try:
    _surf = np.ctypeslib.load_library('libsurf_feature', __path__[0] + '/lib/')
except OSError:
    _surf = np.ctypeslib.load_library('libsurf_feature', '.')
_surf.compute_surf_random.restype = ctypes.c_int
_surf.compute_surf_random.argtypes = [_char_ptr, _int, _int, _int, _float_ptr]


def make_features(image, max_points=1000):
    width, height = image.size
    max_points = min(max_points, height * width)
    points = np.ascontiguousarray(np.zeros((max_points, 64), dtype=np.float32))
    num_pts = _surf.compute_surf_random(image.tostring(),
                                        height,
                                        width,
                                        max_points,
                                        points.ctypes.data_as(_float_ptr))
    return points[:num_pts]

if __name__ == "__main__":
    import time
    tests = ['lena.png']
    for test in tests:
        im = Image.open(test)
        st = time.time()
        print(make_features(im))
        print(time.time() - st)
