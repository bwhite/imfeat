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

"""Bag of Visual Words
"""

__author__ = 'Brandyn A. White <bwhite@cs.umd.edu>'
__license__ = 'GPL V3'

import numpy as np

class BoVW(object):
    def __init__(self, image_feature, dist, normalize, clusters):
        """
        Args:
            image_feature: Has a MODES attribute and a make_features method
            dist: Has a nn method
            normalize: Takes a numpy array and returns a normalized version
            clusters: A list-like object of real-valued list-like objects
        """
        self.image_feature = image_feature
        self.MODES = image_feature.MODES
        self.dist = dist
        self.normalize = normalize
        self.clusters = np.asfarray(clusters)

    def make_features(self, image):
        bovw = np.zeros(len(clusters), dtype=np.int32)
        for feat in image_feature.make_features(image):
            bovw[self.dist.nn(feat)[1]] += 1
        bovw = np.array(bovw, dtype=np.double)
        bovw = normalize(bovw)
        return [bovw]
