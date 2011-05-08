from distutils.core import setup
import re
from distutils.extension import Extension
import numpy as np


def get_cython_version():
    """
    Returns:
        Version as a pair of ints (major, minor)

    Raises:
        ImportError: Can't load cython or find version
    """
    import Cython.Compiler.Main
    match = re.search('^([0-9]+)\.([0-9]+)',
                      Cython.Compiler.Main.Version.version)
    try:
        return map(int, match.groups())
    except AttributeError:
        raise ImportError

# Only use Cython if it is available, else just use the pre-generated files
try:
    cython_version = get_cython_version()
    # Requires Cython version 0.13 and up
    if cython_version[0] == 0 and cython_version[1] < 13:
        raise ImportError
    from Cython.Distutils import build_ext
    source_ext = '.pyx'
    cmdclass = {'build_ext': build_ext}
except ImportError:
    source_ext = '.c'
    cmdclass = {}

ext_modules = [Extension("_imfeat_histogram",
                         ["imfeat/new_histogram/histogram" + source_ext,
                          'imfeat/new_histogram/histogram_aux.c'],
                         extra_compile_args=['-I', np.get_include()]),
               Extension("_imfeat_autocorrelogram",
                         ["imfeat/_autocorrelogram/autocorrelogram" + source_ext,
                          'imfeat/_autocorrelogram/Autocorrelogram.cpp'],
                         extra_compile_args=['-I', np.get_include()])]

setup(name='imfeat',
      cmdclass=cmdclass,
      version='.01',
      packages=['imfeat', 'imfeat._moments', 'imfeat._histogram_joint',
                'imfeat._histogram', 'imfeat._rhog', 'imfeat._rhog_dalal',
                'imfeat._surf', 'imfeat._faces', 'imfeat.new_histogram'],
      package_data={'imfeat._moments': ['lib/*.so'],
                    'imfeat._rhog': ['lib/*.so'],
                    'imfeat._surf': ['lib/*.so'],
                    'imfeat._histogram_joint': ['lib/*.so'],
                    'imfeat._histogram': ['lib/*.so'],
                    'imfeat._faces': ['data/*']},
      ext_modules=ext_modules)
