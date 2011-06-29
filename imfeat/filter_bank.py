#!/usr/bin/env python
# (C) Copyright 2010 Brandyn A. White
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

"""Generates filter_bank features
"""

__author__ = 'Brandyn A. White <bwhite@cs.umd.edu>'
__license__ = 'GPL V3'

import numpy as np
import warnings

MODES = ['L']
_filters = None
_caches = None
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


def _ge_pow2(val):
    """
    Args:
        val: Integer of interest

    Returns:
        The nearest power of 2 greater or equal to this
    """
    return 2 ** int(np.ceil(np.log2(val)))


def _make_default():
    filter_func = gabor_schmid
    params = ({'tau': t, 'sigma': s, 'radius': 20}
              for t in range(1, 8, 2)
              for s in range(5, 10, 2))
    return filter_func, params


def _setup(filter_func, params):
    global _filters, _filter_func, _params, _caches
    if _filters != None and filter_func == _filter_func and params == _params:
        return
    if filter_func == None and params == None:
        filter_func, params = _make_default()
    _filters = [filter_func(**param) for param in params]
    _caches = [{} for x  in range(len(_filters))]
    _filter_func = filter_func
    _params = params
    for x in _filters[1:]:  # We assume all filters are the same shape
        assert(_filters[0].shape == x.shape)


def _centered(arr, newsize):
    # Return the center newsize portion of the array.
    newsize = np.asarray(newsize)
    currsize = np.array(arr.shape)
    startind = (currsize - newsize) / 2
    endind = startind + newsize
    myslice = [slice(startind[k], endind[k]) for k in range(len(endind))]
    return arr[tuple(myslice)]


def fftconvolve_cache(in1, in2, mode="full", cache1=None, cache2=None):
    """Convolve two N-dimensional arrays using FFT. See convolve.

    Modified by Brandyn: Uses a cache for the second arg to prevent
    recomputing fft.

    """
    from scipy.signal import fftn, ifftn
    s1 = np.array(in1.shape)
    s2 = np.array(in2.shape)
    complex_result = False
    size = s1+s2-1

    # Always use 2**n-sized FFT
    fsize = 2**np.ceil(np.log2(size))
    fsize_k = tuple(fsize.tolist())
    # Brandyn: Start Mod
    cache1 = {} if cache1 == None else cache1
    cache2 = {} if cache2 == None else cache2
    try:
        IN1 = cache1[fsize_k].copy()
    except KeyError:
        cache1[fsize_k] = fftn(in1, fsize)
        IN1 = cache1[fsize_k].copy()
    try:
        IN1 *= cache2[fsize_k]
    except KeyError:
        cache2[fsize_k] = fftn(in2, fsize)
        IN1 *= cache2[fsize_k]
    # Brandyn: End Mod
    fslice = tuple([slice(0, int(sz)) for sz in size])
    ret = ifftn(IN1)[fslice].copy()
    del IN1
    if not complex_result:
        ret = ret.real
    if mode == "full":
        return ret
    elif mode == "same":
        if np.product(s1, axis=0) > np.product(s2, axis=0):
            osize = s1
        else:
            osize = s2
        return _centered(ret, osize)
    elif mode == "valid":
        return _centered(ret, abs(s2 - s1) + 1)


def _convolve(image, cache1, filt_cache):
    filt, cache2 = filt_cache
    with warnings.catch_warnings():
        warnings.simplefilter("ignore")
        return fftconvolve_cache(image, filt, 'valid',
                                 cache1=cache1, cache2=cache2)


def _make_convs(image, filter_func=None, params=None):
    _setup(filter_func, params)
    image = np.asfarray(image)
    cur_cache = {}
    return [_convolve(image, cache1=cur_cache, filt_cache=filt_cache)
            for filt_cache in zip(_filters, _caches)]


def make_features(image, filter_func=None, params=None):
    convs = _make_convs(image, filter_func, params)
    convs = [x.ravel() for x in convs]
    return np.array(convs).T


def make_texton(image, dist, clusters, filter_func=None, params=None):
    """Generates a texton image.

    Args:
        image: PIL Image
        dist: Has a nn method
        clusters: A list-like object of real-valued list-like objects
        filter_func: Function that produces a filter given an entry in params
        params: A set of filter parameters

    Returns:
        2D numpy array of texton indeces
    """
    convs = _make_convs(image, filter_func, params)
    shape = convs[0].shape
    convs = np.asfarray(convs)
    texton = np.zeros(shape)
    for i in range(shape[0]):
        for j in range(shape[1]):
            texton[i, j] = dist.nn(clusters, convs[:, i, j])[1]
    return texton


def _demo():
    import distpy
    import pickle
    import Image
    import glob
    import os
    import matplotlib.pyplot as mp
    clust_fn = ('/home/brandyn/projects/constrained_visual_analysis/'
                'box_seg_classifier/cluster/vocab_filter.pkl99')
    image_fns = glob.glob(('/home/brandyn/data/msrc/working/'
                           'MSRC_ObjCategImageDatabase_v2/Images/*.bmp'))
    out_dir = '/home/brandyn/playground/texton/'
    try:
        os.makedirs(out_dir)
    except OSError:
        pass
    for image_fn in image_fns:
        clust = pickle.load(open(clust_fn))
        image = Image.open(image_fn).convert('L')
        mp.clf()
        mp.imshow(make_texton(image, distpy.L2Sqr(), clust))
        mp.savefig(out_dir + os.path.basename(image_fn) + '.png')

if __name__ == '__main__':
    _demo()
