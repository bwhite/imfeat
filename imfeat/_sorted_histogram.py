import imfeat


class SortedHistogram(imfeat.Histogram):

    def __init__(self, *args, **kw):
        super(SortedHistogram, self).__init__(*args, **kw)

    def __call__(self, image):
        out = super(SortedHistogram, self).__call__(image)
        out.sort()
        return out
