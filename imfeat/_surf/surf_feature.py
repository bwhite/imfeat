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
_surf.compute_surf_descriptors.restype = ctypes.c_int
_surf.compute_surf_descriptors.argtypes = [_char_ptr, _int, _int, _int, _float_ptr]
_surf.compute_surf_points.restype = ctypes.c_int
_surf.compute_surf_points.argtypes = [_char_ptr, _int, _int, _int, _float_ptr, _int_ptr, _int_ptr, _int_ptr, _float_ptr, _bool_ptr,_float_ptr]


def make_features(image, max_points=100):
    width, height = image.size
    points = np.ascontiguousarray(np.zeros((max_points, 64), dtype=np.float32))
    num_pts = _surf.compute_surf_descriptors(image.tostring(),
                                             height,
                                             width,
                                             max_points,
                                             points.ctypes.data_as(_float_ptr))
    return points[:num_pts]

def make_points(image, max_points=100):
    width, height = image.size
    points = np.ascontiguousarray(np.zeros((max_points, 64), dtype=np.float32))
    x = np.ascontiguousarray(np.zeros(max_points, dtype=np.int32))
    y = np.ascontiguousarray(np.zeros(max_points, dtype=np.int32))
    scale = np.ascontiguousarray(np.zeros(max_points, dtype=np.int32))
    orientation = np.ascontiguousarray(np.zeros(max_points, dtype=np.float32))
    sign = np.ascontiguousarray(np.zeros(max_points, dtype=np.bool))
    cornerness = np.ascontiguousarray(np.zeros(max_points, dtype=np.float32))
    num_pts = _surf.compute_surf_points(image.tostring(),
                                        height,
                                        width,
                                        max_points,
                                        points.ctypes.data_as(_float_ptr),
                                        x.ctypes.data_as(_int_ptr),
                                        y.ctypes.data_as(_int_ptr),
                                        scale.ctypes.data_as(_int_ptr),
                                        orientation.ctypes.data_as(_float_ptr),
                                        sign.ctypes.data_as(_bool_ptr),
                                        cornerness.ctypes.data_as(_float_ptr))
    out = [{'x': x[i], 'y': y[i], 'scale': scale[i], 'desc': points[i],
            'orientation': orientation[i], 'sign': sign[i],
            'cornerness': cornerness[i]} for i in range(num_pts)]
    return out


if __name__ == "__main__":
    tests = ['real_test.jpg', 'real_test2.jpg']
    for test in tests:
        im = Image.open(test)
        st = time.time()
        print(make_features(im))
        print(make_feature_points(im))
        print(time.time() - st)
