import imfeat
import numpy as np
import cPickle as pickle
import cv2


def convert_leaves_all_probs_pred(image, leaves, all_probs, num_leaves):
    import kontort
    preds = []  # NOTE(brandyn): This has the ILP turned off
    preds = np.ascontiguousarray(preds, dtype=np.float64)
    out0 = kontort.convert_labels_to_integrals(leaves, num_leaves)
    out1 = kontort.convert_all_probs_to_integrals(all_probs)
    return preds, np.ascontiguousarray(np.dstack([out0, out1]))


class TextonBase(imfeat.BaseFeature):

    def __init__(self):
        super(TextonBase, self).__init__({'type': 'numpy', 'dtype': 'uint8', 'mode': 'bgr'})
        import kontort
        from imfeat._texton.msrc_model import data
        self.tp = kontort.TextonPredict(pickle.loads(data[1][1]))  # NOTE(brandyn): TP = 1 and TP2 = 0 as that is how the names were sorted
        self.tp2 = kontort.IntegralPredict(pickle.loads(data[0][1]))
        self.num_classes = 21
        self.grad = imfeat.GradientHistogram()

    def _make_masks(self, image):
        image = imfeat.convert_image(image, [{'type': 'numpy', 'mode': 'bgr', 'dtype': 'uint8'}])
        image_gray = imfeat.convert_image(image, [{'type': 'numpy', 'mode': 'gray', 'dtype': 'uint8'}])
        image_gradient = np.array(self.grad.make_feature_mask(np.array(image_gray, dtype=np.float32) / 255.) * 255, dtype=np.uint8)
        image_lab = imfeat.convert_image(image, [{'type': 'numpy', 'mode': 'lab', 'dtype': 'uint8'}])
        return np.ascontiguousarray(np.dstack([image, image_lab, image_gradient]), dtype=np.uint8)

    def _predict(self, image):
        # Makes the max size length 320
        max_side = 320
        image = imfeat.convert_image(image, self.MODES)
        sz = np.array([image.shape[1], image.shape[0]])
        sz = np.array(max_side * sz / float(np.max(sz)), dtype=np.int)
        image = cv2.resize(image, tuple(sz))
        image = self._make_masks(image)
        # Predict using both trees
        max_classes1, max_probs1, leaves1, all_probs1 = self.tp.predict(image, leaves=True, all_probs=True)
        pred_integrals = convert_leaves_all_probs_pred(image, leaves1, all_probs1, self.tp.num_leaves)
        max_classes2, max_probs2, all_probs2 = self.tp2.predict(pred_integrals, all_probs=True)
        return max_classes1, max_probs1, leaves1, max_classes2, max_probs2, all_probs2


class TextonSpatialHistogram(TextonBase):

    def __init__(self, levels=1, other_class_thresh=None, norm=True):
        super(TextonSpatialHistogram, self).__init__()
        self.levels = levels
        self.other_class_thresh = other_class_thresh
        self.norm = norm

    def __call__(self, image):
        image = self.convert(image)
        import time
        st = time.time()
        max_classes1, max_probs1, leaves1, max_classes2, max_probs2, all_probs2 = self._predict(image)
        num_classes = self.num_classes
        if self.other_class_thresh is not None:
            max_classes2[max_probs2 < self.other_class_thresh] = self.num_classes
            num_classes += 1
        out = imfeat.spatial_pyramid(max_classes2, self.levels, num_classes)
        out = np.asfarray(out)
        if self.norm:
            out /= np.sum(out)
        print(time.time() - st)
        return out


class TextonHistogram(TextonSpatialHistogram):

    def __init__(self, levels=1):
        super(TextonHistogram, self).__init__(levels=levels)


class TextonImage(TextonSpatialHistogram):

    def __init__(self, size=4):
        levels = np.log2(size)
        assert levels == int(levels)  # NOTE(brandyn): Must be power of 2
        super(TextonImage, self).__init__(levels=int(levels) + 1)
        self._keep_dims = int(size ** 2 * self.num_classes)

    def __call__(self, image):
        image = self.convert(image)
        shist = super(TextonImage, self).__call__(image)
        # NOTE(brandyn): The function computes more than we need, later add another parameter to stop it early
        shist = shist[-self._keep_dims:]
        return np.asfarray(np.argmax(shist.reshape((shist.size / self.num_classes, self.num_classes)), 1))
