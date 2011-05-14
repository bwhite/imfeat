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
"""Test
"""
__author__ = 'Brandyn A. White <bwhite@cs.umd.edu>'
__license__ = 'GPL V3'

import unittest

import numpy as np
import Image
import pickle

import imfeat
import distpy
import random


def mk(make_features, frac=.1):

    def sample_make_features(x):
        features = make_features(x)
        return random.sample(features, int(len(features) * frac))
    return sample_make_features


class Test(unittest.TestCase):

    def test_0(self):
        with open('voc_bovw_texton.pkl') as clusters_fp:
            clusters = np.asfarray(pickle.load(clusters_fp))
        normalize = lambda x: x / np.linalg.norm(x, 1)
        imfeat.filter_bank.make_features = mk(imfeat.filter_bank.make_features)
        bovw = imfeat.BoVW(imfeat.filter_bank, distpy.L2Sqr(), normalize,
                           clusters)
        lena = Image.open('test_images/lena.jpg').convert('L')
        print(bovw.make_features(lena))

if __name__ == '__main__':
    unittest.main()
