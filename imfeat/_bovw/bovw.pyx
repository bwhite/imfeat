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

"""Bag of Visual Words
"""

__author__ = 'Brandyn A. White <bwhite@cs.umd.edu>'
__license__ = 'GPL V3'

import numpy as np
import scipy as sp
import cv
import numpy as np
cimport numpy as np
cimport imfeat
import random


cdef extern from "bovw_aux.h":
    void bovw_fast_hist(np.int32_t *neighbor_map, np.int32_t *bovw, int height, int width, int bins, int level)
    void bovw_fast_sum(np.int32_t *bovw_fine, np.int32_t *bovw_coarse, int height_fine, int width_fine, int bins)

cpdef spatial_pyramid(np.ndarray[np.int32_t, ndim=2, mode='c'] label_image, int levels, int max_values):
    assert 0 < levels
    cdef np.ndarray shist = np.zeros((2 ** (levels - 1), 2 ** (levels - 1), max_values), dtype=np.int32)
    cdef np.ndarray[np.int32_t, ndim=3, mode='c'] shist_coarse
    assert np.max(label_image) < max_values
    assert 0 <= np.min(label_image)
    bovw_fast_hist(<np.int32_t *>label_image.data, <np.int32_t *>shist.data,
                   label_image.shape[0], label_image.shape[1], max_values, levels - 1)
    out = [shist]
    for x in range(levels - 1):
        shist_coarse = np.zeros((shist.shape[0] / 2, shist.shape[1] / 2, shist.shape[2]), dtype=np.int32)
        assert shist_coarse.shape[2] == shist.shape[2]
        bovw_fast_sum(<np.int32_t *>shist.data, <np.int32_t *>shist_coarse.data, shist.shape[0], shist.shape[1], shist.shape[2])
        shist = shist_coarse
        out.append(shist)
    # Normalize and scale the bins
    return np.hstack([(y.ravel() / float(np.sum(y))) * 2 ** x for x, y in enumerate(out[::-1])])


cdef class BoVW(imfeat.BaseFeature):
    cdef object feature_point_func, dist, normalize
    cdef int levels
    cdef int max_values
    
    def __init__(self, feature_point_func, max_values, levels=1):
        """
        Args:
            feature_point_func: Produces a mask
            levels: Number of levels, each level is a power of 2 # of segments
        """
        self.feature_point_func = feature_point_func
        self.max_values = max_values
        self.levels = levels

    @classmethod
    def cluster(cls, images, feature_point_func, num_clusters, max_image_points=None):
        try:
            import scipy.cluster
        except RuntimeError:  # NOTE(brandyn): Fixes problems where the home directory is not specified
            import os
            import tempfile
            os.environ['HOME'] = tempfile.gettempdir()
            import scipy.cluster

        points = []
        for image in images:
            p = feature_point_func(image)
            if max_image_points is not None:
                p = random.sample(p, min(max_image_points, len(p)))
            points.append(p)
        points = np.vstack(points)
        return sp.cluster.vq.kmeans(points, num_clusters)[0]

    def __call__(self, image):
        image = self.convert(image)
        cdef np.ndarray neighbor_map = np.ascontiguousarray(self.feature_point_func(image), dtype=np.int32)
        return np.asfarray(spatial_pyramid(neighbor_map, self.levels, self.max_values))
