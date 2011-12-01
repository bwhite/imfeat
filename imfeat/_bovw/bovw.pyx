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


cdef extern from "bovw_aux.h":
    void bovw_fast_hist(np.int32_t *neighbor_map, np.int32_t *bovw, int height, int width, int bins, int level)
    void bovw_fast_sum(np.int32_t *bovw_fine, np.int32_t *bovw_coarse, int height_fine, int width_fine, int bins)

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
    def cluster(cls, images, feature_point_func, num_clusters):
        try:
            import scipy.cluster
        except RuntimeError:  # NOTE(brandyn): Fixes problems where the home directory is not specified
            import os
            import tempfile
            os.environ['HOME'] = tempfile.gettempdir()
            import scipy.cluster
        points = np.vstack([feature_point_func(image) for image in images])
        return sp.cluster.vq.kmeans(points, num_clusters)[0]

    def make_features(self, image):
        cdef np.ndarray bovw = np.zeros((2 ** (self.levels - 1), 2 ** (self.levels - 1), self.max_values), dtype=np.int32)
        cdef np.ndarray bovw_coarse
        cdef np.ndarray neighbor_map = np.ascontiguousarray(self.feature_point_func(image), dtype=np.int32)
        assert np.max(neighbor_map) < self.max_values
        assert 0 <= np.min(neighbor_map)
        bovw_fast_hist(<np.int32_t *>neighbor_map.data, <np.int32_t *>bovw.data,
                       neighbor_map.shape[0], neighbor_map.shape[1], self.max_values, self.levels - 1)
        # Build the output array
        out = [bovw]
        for x in range(self.levels - 1):
            bovw_coarse = np.zeros((bovw.shape[0] / 2, bovw.shape[1] / 2, bovw.shape[2]))
            assert bovw_coarse.shape[2] == bovw.shape[2]
            bovw_fast_sum(<np.int32_t *>bovw.data, <np.int32_t *>bovw_coarse.data, bovw.shape[0], bovw.shape[1], bovw.shape[2])
            bovw = bovw_coarse
            out.append(bovw)
        # Normalize and scale the bins
        out = [(y.ravel() / float(np.sum(y))) * 2 ** x for x, y in enumerate(out[::-1])]
        return [np.hstack(out)]
