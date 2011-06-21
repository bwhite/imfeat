import imfeat
import numpy as np


class SpatialHistogram(imfeat.Histogram):

    def __init__(self, num_rows=1, num_cols=1, *args, **kw):
        super(SpatialHistogram, self).__init__(*args, **kw)
        self._num_rows = int(num_rows)
        self._num_cols = int(num_cols)

    def make_features(self, image_cv):
        output_size = step_delta = (image_cv.height / self._num_rows, image_cv.width / self._num_cols)
        bgen = imfeat.BlockGenerator(image_cv, imfeat.CoordGeneratorRect,
                                     output_size=output_size, step_delta=step_delta)
        return [np.hstack([super(SpatialHistogram, self).make_features(image)[0] for image, _ in bgen])]
