import Image
import numpy as np
import time

import moments


def make_features(image):
    if image.mode != 'RGB':
        return None
    return [moments.compute(3,
                            image.size[0] * image.size[1],
                            1,
                            image.tostring())[1:].tostring()]

if __name__ == "__main__":
    tests = ['real_test.jpg', 'real_test2.jpg']
    for test in tests:
        im = Image.open(test)
        st = time.time()
        print(make_features(im))
        print(time.time() - st)
