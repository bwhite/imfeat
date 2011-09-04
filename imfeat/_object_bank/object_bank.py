import cv
import numpy as np
import os
import imfeat
import shutil
import tempfile
import subprocess
from . import __path__


class ObjectBank(imfeat.BaseFeature):

    def __init__(self):
        super(ObjectBank, self).__init__()
        self.MODES = [('opencv', 'bgr', 8)]
        self.path = (__path__[0] + '/data/')
        if not os.path.exists(self.path + 'OBmain'):
            raise OSError('File not found! [%s]' % (self.path + 'OBmain'))
        self._temp_dir_in = tempfile.mkdtemp()
        self._temp_dir_out = tempfile.mkdtemp()
        self.image_path = os.path.join(self._temp_dir_in, '00000.jpg')
        self.feat_path = os.path.join(self._temp_dir_out, '00000.jpg.feat')

    def __del__(self):
        pass
        #shutil.rmtree(self._temp_dir_in)
        #shutil.rmtree(self._temp_dir_out)

    def make_features(self, image):
        cv.SaveImage(self.image_path, image)
        orig_dir = os.path.abspath('.')
        try:
            os.chdir(self.path)
            print(os.path.abspath('.'))
            print(os.listdir('.'))
            cmd = './OBmain %s/ %s/' % (self._temp_dir_in, self._temp_dir_out)
            print('======================   %s -----------------------' % cmd)
            subprocess.call(cmd.split())
        finally:
            os.chdir(orig_dir)
        try:
            with open(self.feat_path) as fp:
                return [np.asfarray([float(x.rstrip()) for x in fp])]
        finally:
            os.remove(self.feat_path)
