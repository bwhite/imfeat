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
import numpy as np
cimport numpy as np
cimport imfeat


cdef extern from "dedupe_aux.h":
    void dedupe_image_to_feat(np.uint8_t *image_lab, np.float64_t *hist_out, int height, int width)

cdef class Dedupe(imfeat.BaseFeature):
    
    def __init__(self):
        super(Dedupe, self).__init__({'type': 'numpy', 'dtype': 'uint8', 'mode': 'lab'})

    def __reduce__(self):
        return (Dedupe, ())

    def __call__(self, image):
        cdef np.ndarray image_in = self.convert(image)
        cdef np.ndarray hist = np.zeros(384, dtype=np.double)
        dedupe_image_to_feat(<np.uint8_t *>image_in.data, <np.float64_t *> hist.data, image.shape[0], image.shape[1])
        return hist
