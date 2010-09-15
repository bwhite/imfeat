import numpy as np

def gabor_schmid(tau=2, sigma=1, radius=5):
    """Gabor-like filter maker
    
    From "Constructing models for content-based image retrieval"
    
    Args:
        tau: Controls frequency
        sigma: Controls gaussian size

    Returns:
        Numpy array holding the filter
    """
    tau = float(tau)
    sigma = float(sigma)
    sz = 2 * radius + 1
    x, y = np.meshgrid(range(-radius, sz - radius),
                       range(-radius, sz - radius))
    x *= x
    y *= y
    x += y
    out = np.exp(-x / (2. * sigma * sigma))
    out *= np.cos(np.pi * tau * np.sqrt(x) / sigma)
    out -= np.mean(out.ravel())
    out /= np.std(out.ravel())
    return out
