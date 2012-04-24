import imfeat
import numpy as np


class SpatialHistogram(imfeat.Histogram):

    def __init__(self, num_rows=1, num_cols=1, *args, **kw):
        super(SpatialHistogram, self).__init__(*args, **kw)
        self._num_rows = int(num_rows)
        self._num_cols = int(num_cols)

    def convert(self, image, converted=True):
        if converted:
            return image
        return super(SpatialHistogram, self).convert(image)

    def __call__(self, image_np):
        image_np = self.convert(image_np, converted=False)
        height, width = image_np.shape[:2]
        output_size = step_delta = (height / self._num_rows, width / self._num_cols)
        bgen = imfeat.BlockGenerator(image_np, imfeat.CoordGeneratorRect,
                                     output_size=output_size, step_delta=step_delta)
        return np.hstack([super(SpatialHistogram, self).__call__(image) for image, _ in bgen])
