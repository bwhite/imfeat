import imfeat


cdef class BaseFeature(object):
    cdef public object MODES

    def __init__(self, modes=None):
        super(BaseFeature, self).__init__()
        self.MODES = modes

    def __call__(self, image):
        raise NotImplementedError

    def convert(self, image):
        if self.MODES is None:
            return image
        return imfeat.convert_image(image, self.MODES)

    def make_feature_mask(self, image):
        pass

    def make_bow_mask(self, image, clusters):
        """Compute a 2D BoVW mask as a numpy array

        The resulting mask is not necessarily the same shape as the input
        but they are as they are produced by the feature.

        Args:
            image: Image input
            clusters: 2D numpy array (num_clusters, num_dims)

        Returns:
            Numpy array with dims (rows, cols)
        """
        raise NotImplementedError

    def compute_dense(self, image):
        """Compute a set of dense feature vectors

        Intended for use where localization of the features is not necessary

        Args:
            image: Image input

        Returns:
            Numpy array with dims (num_feat, num_dims)
        """
        raise NotImplementedError

# 1. Feature has a test
# 2. Feature calls convert(image)
# 3. Feature call super
