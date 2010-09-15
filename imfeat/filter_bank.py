import numpy as np
import scipy as sp
import scipy.signal
import Image
from . import __path__

MODES = ['L']
_filters = None
_filter_func = None
_params = None


def gabor_schmid(tau=2, sigma=1, radius=5):
    """Gabor-like filter maker
    
    From "Constructing models for content-based image retrieval"
    
    Args:
        tau: Controls frequency
        sigma: Controls gaussian size
        radius: Controls the finals size of the filter, 2*radius+1

    Returns:
        Numpy array holding the filter
    """
    tau = float(tau)
    sigma = float(sigma)
    sz = 2 * radius + 1
    x, y = np.meshgrid(range(-radius, sz - radius),
                       range(-radius, sz - radius))
    x *= x
    y *= y
    x += y
    out = np.exp(-x / (2. * sigma * sigma))
    out *= np.cos(np.pi * tau * np.sqrt(x) / sigma)
    out -= np.mean(out.ravel())
    out /= np.std(out.ravel())
    return out


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
