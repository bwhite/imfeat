import Image
import numpy as np


def make_features(image):
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
