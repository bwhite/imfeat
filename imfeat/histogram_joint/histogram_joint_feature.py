import Image
import numpy as np

import histogram_joint

def make_features(image):
    # If it is a GIF, then convert to RGB
    if image.mode == 'P':
        image = image.convert('RGB')
    if image.mode != 'RGB':
        return None
    return [np.array(histogram_joint.compute(image.size[0] * image.size[1], 16, image.tostring()), dtype=np.float32).tostring()]

if __name__ == "__main__":
    import time
    tests = ['../test0.jpg']
    for test in tests:
        im = Image.open(test)
        st = time.time()
        print(make_features(im))
        print(time.time() - st)
