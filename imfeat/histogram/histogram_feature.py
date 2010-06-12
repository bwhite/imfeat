import Image
import numpy as np

from hadoopy import reporter
from Histogram import Histogram

def make_features(image):
    # If it is a GIF, then convert to RGB
    if image.mode == 'P':
        image = image.convert('RGB')
    # If it is Gray, then drop it
    if image.mode != 'RGB':
        reporter.counter('feature_computation', 'NOTRGB')
        return None
    h = Histogram(3, image.size[0]*image.size[1], 16)
    return [np.array(h.compute(image.tostring()), dtype=np.float32).tostring()]

if __name__ == "__main__":
    import time
    tests = ['img.jpg']
    for test in tests:
        im = Image.open(test)
        st = time.time()
        print(make_features(im))
        print(time.time() - st)
