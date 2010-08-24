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

import imfeat


class Test(unittest.TestCase):
    def test_0(self):
        # Find all features
        lena = Image.open('lena.jpg')
        for feature_module in dir(imfeat):
            if 'make_features' in dir(getattr(imfeat, feature_module)):
                print(feature_module)
                val = imfeat.compute(getattr(imfeat, feature_module), lena)
                print(len(val))
                try:
                    print(len(val[0]))
                except IndexError:
                    pass
                print(val)

if __name__ == '__main__':
    unittest.main()
