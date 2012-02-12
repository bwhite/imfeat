import cv
import numpy as np
import os
import imfeat
import shutil
import tempfile
import subprocess
import glob
import stat
from . import __path__


class ObjectBank(imfeat.BaseFeature):

    def __init__(self):
        super(ObjectBank, self).__init__()
        self.MODES = [('opencv', 'bgr', 8)]
        self._temp_root = tempfile.mkdtemp()
        self._temp_dir_in = self._temp_root + '/in'
        self._temp_dir_out = self._temp_root + '/out'
        try:
            os.makedirs(self._temp_dir_in)
        except OSError:
            pass
        try:
            os.makedirs(self._temp_dir_out)
        except OSError:
            pass
        self.model_path = '%s/models' % self._temp_dir_out
        if os.path.exists('OBmain'):  # Use curdir
            self.path = os.path.abspath('.')
            self.ob_path = './OBmain'
            try:
                os.chmod(self.ob_path, stat.S_IXUSR | stat.S_IRUSR)
            except OSError:
                pass
            open(self.model_path, 'w').write('\n'.join(glob.glob('*.text')) + '\n')
        else:
            self.path = __path__[0]
            shutil.copy(os.path.join(self.path, 'data/OBmain'), self._temp_root)
            self.ob_path = os.path.join(self._temp_root, 'OBmain')
            try:
                os.chmod(self.ob_path, stat.S_IXUSR | stat.S_IRUSR)
            except OSError:
                pass
            open(self.model_path, 'w').write('\n'.join(glob.glob(self.path + '/data/models/*.text')) + '\n')
            if not os.path.exists(self.path + '/data/OBmain'):
                raise OSError('File not found! [%s]' % (self.path + 'OBmain'))
        self.image_path = os.path.join(self._temp_dir_in, '00000.jpg')
        self.feat_path = os.path.join(self._temp_dir_out, '00000.jpg.feat')

    def __del__(self):
        shutil.rmtree(self._temp_root)

    def make_features(self, image):
        cv.SaveImage(self.image_path, image)
        orig_dir = os.path.abspath('.')
        try:
            os.chdir(self.path)
            cmd = '%s -M %s %s/ %s/' % (self.ob_path, self.model_path, self._temp_dir_in, self._temp_dir_out)
            subprocess.call(cmd.split())
        finally:
            os.chdir(orig_dir)
        try:
            with open(self.feat_path) as fp:
                return [np.asfarray([float(x.rstrip()) for x in fp])]
        finally:
            os.remove(self.image_path)
            os.remove(self.feat_path)
