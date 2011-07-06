from main import compute, compute_points, convert_image
from _imfeat import BaseFeature
import dummy
import rand
from _gradient_hist import GradientHistogram
from _imfeat_autocorrelogram import Autocorrelogram
from _imfeat_histogram import Histogram
from _imfeat_moments import Moments
from _imfeat_rhog import RHOG
from _imfeat_hog_latent import HOGLatent
from _imfeat_gist import GIST
from _imfeat_block_generator import CoordGeneratorBase, CoordGeneratorRect, BlockGenerator, CoordGeneratorRectRotate
#from _rhog_dalal import rhog as rhog_dalal
from _bovw import BoVW
import filter_bank
from _faces.faces import Faces
from _spatial_histogram import SpatialHistogram
from _meta import MetaFeature
try:  # Try to import these packages to aid pyinstaller
    import numpy as _np
    import cv as _cv
    import Image
    import cv2 as _cv2
except ImportError:
    pass
