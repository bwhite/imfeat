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
import multiprocessing
from scipy.signal import convolve2d

MODES = ['L']
_filters = None
_filter_func = None
_params = None
_pool = None


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
    params = ({'tau': t, 'sigma': s, 'radius': 20}
              for t in range(1, 8)
              for s in range(5, 10))
    return filter_func, params


def _setup(filter_func, params):
    global _filters, _filter_func, _params, _pool
    if _pool == None:
        _pool = multiprocessing.Pool()
    if _filters != None and filter_func == _filter_func and params == _params:
        return
    if filter_func == None and params == None:
        filter_func, params = _make_default()
    _filters = [filter_func(**param) for param in params]
    _filter_func = filter_func
    _params = params
    for x in _filters[1:]:  # We assume all filters are the same shape
        assert(_filters[0].shape == x.shape)


def _convolve(image_filt):
    image, filt = image_filt
    with warnings.catch_warnings():
        warnings.simplefilter("ignore")
        return convolve2d(image, filt, 'valid')


def _make_convs(image, filter_func=None, params=None):
    _setup(filter_func, params)
    image = np.asfarray(image)
    return _pool.map(_convolve, zip([image] * len(_filters), _filters))


def make_features(image, filter_func=None, params=None):
    convs = _make_convs(image, filter_func, params)
    convs = [x.ravel() for x in convs]
    return np.asfarray([x for x in zip(*convs)])


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
    import random
    import os
    import matplotlib.pyplot as mp
    clust_fn = '/home/brandyn/projects/constrained_visual_analysis/box_seg_classifier/cluster/vocab_filter.pkl99'
    image_fns = glob.glob('/home/brandyn/data/msrc/working/MSRC_ObjCategImageDatabase_v2/Images/*.bmp')
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
    
