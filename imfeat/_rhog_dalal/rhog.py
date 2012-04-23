import numpy as np
import tempfile
import cv2
import subprocess
import os
import imfeat

from . import __path__


class RHOGDalal(imfeat.BaseFeature):

    def __init__(self):
        super(RHOGDalal, self).__init__({'type': 'numpy', 'mode': 'bgr', 'dtype': 'uint8'})

    def __call__(image, fn=None, options=None):
        try:
            param = ' '.join(['--%s %s' % x for x in options.items()])
        except AttributeError:
            param = ''
        if fn:
            img_fn = fn
            img_fd = None
        else:
            img_fd, img_fn = tempfile.mkstemp(suffix='.rhog.png')
            cv2.imwrite(img_fn, image)
        desc_fd, desc_fn = tempfile.mkstemp(suffix='.rhog.desc')
        mat_fd, mat_fn = tempfile.mkstemp(suffix='.rhog.mat.desc')
        cmd = '%s/dump_rhog %s --infile %s --outfile %s' % (__path__[0], param, img_fn, desc_fn)
        subprocess.call(cmd.split())
        cmd = '%s/dump4svmlearn -p %s --outfile %s -f BiMatlab' % (__path__[0], desc_fn, mat_fn)
        subprocess.call(cmd.split())
        out = [np.fromstring(os.fdopen(mat_fd).read()[12:], dtype=np.float32)]

        def cls(fd, fn):
            if fd == None:
                return
            try:
                os.close(fd)
            except OSError:
                pass
            try:
                os.remove(fn)
            except OSError:
                pass
        cls(mat_fd, mat_fn)
        cls(desc_fd, desc_fn)
        cls(img_fd, img_fn)
        return out
