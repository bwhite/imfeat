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
import types

import imfeat


class Test(unittest.TestCase):
    def test_0(self):
        # Find all features
        lena = Image.open('lena.jpg')
        for feature_module in dir(imfeat):
            mod = getattr(imfeat, feature_module)
            if isinstance(mod, types.TypeType):
                print('Skipping [%s] as it is a class' % feature_module)
                continue
            if feature_module == 'rhog_dalal':
                print('Skipping [%s] as it is unsupported' % feature_module)
                continue
            if 'make_features' in dir(mod):
                print(feature_module)
                val = np.asfarray(imfeat.compute(mod, lena))
                print(len(val))
                try:
                    print(len(val[0]))
                except IndexError:
                    pass
                print(val)

if __name__ == '__main__':
    unittest.main()
