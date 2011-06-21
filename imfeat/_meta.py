import imfeat
import numpy as np


class MetaFeature(imfeat.BaseFeature):

    def __init__(self, *features):
        super(MetaFeature, self).__init__()
        self._features = features

    def make_features(self, image):
        return [np.hstack([imfeat.compute(f, image)[0] for f in self._features])]
