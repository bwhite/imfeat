import numpy as np
import Image
from imfeat.filter_bank.gabor import gabor_schmid
from . import __path__

MODES = ['L']
_filters = None
_filter_func = None
_params = None

def _make_default():
    filter_func = gabor_schmid
    params = ({'tau': t, 'sigma': s} 
              for t in range(1, 8)
              for s in range(1, 5))
    return filter_func, params


def _setup(filter_func, params):
    global _filters
    if _filters != None and filter_func == _filter_func and params == _params:
        return
    if filter_func == None and params == None:
        filter_func, params = _make_default()
    _filters = [filter_func(**param) for param in params]
    _filter_func = filter_func
    _params = params
    for x in _filters[1:]:  # We assume all filters are the same shape
        assert(_filters[0].shape == x.shape)


def make_features(image, filter_func=None, params=None):
    _setup(filter_func, params)
    image = np.asfarray(image)
    convs = [sp.signal.convolve2d(image, filt, 'valid').ravel()
             for filt in _filters]
    convs = [np.asfarray(x) for x in zip(*convs)]
    return convs


if __name__ == "__main__":
    import time
    tests = ['lena.png']
    for test in tests:
        im = Image.open(test)
        st = time.time()
        print(make_features(im))
        print(time.time() - st)
