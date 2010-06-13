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

"""Helper functions for feature computation
"""

__author__ = 'Brandyn A. White <bwhite@cs.umd.edu>'
__license__ = 'GPL V3'

def compute(feature_module, image, *args, **kw):
    """Compute features while performing conversions and checks.

    Args:
        feature_module: A module that has a make_features function.
        image: A PIL image.
        args: Optional positional arguments to be passed on.
        kw: Optional keyword arguments to be passed on.

    Returns:
        Feature computation output.
    """

    try:
        if image.mode not in feature_module.MODES:
            image = image.convert(feature_module.MODES[0])
    except AttributeError:
        pass
    return feature_module.make_features(image, *args, **kw)
