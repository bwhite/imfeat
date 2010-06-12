import numpy as np
import random

def make_features(image, dim=64, num_feat=300):
    return [np.random.random(dim).astype(np.float32).tostring() for x in range(num_feat)]
