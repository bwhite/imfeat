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
    import Cython.Compiler.Main as cymain
    match = re.search('^([0-9]+)\.([0-9]+)', cymain.Version.version)
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
    cmdclass = { 'build_ext': build_ext }
except ImportError:
    source_ext = '.c'
    cmdclass = {}

ext_modules = [Extension("_imfeat",
                         ["imfeat/imfeat" + source_ext],
                         extra_compile_args=['-I', np.get_include()]),
               Extension("_imfeat_conversion",
                         ["imfeat/_conversion" + source_ext],
                         extra_compile_args=['-I', np.get_include()]),
               Extension("_imfeat_histogram",
                         ["imfeat/_histogram/histogram" + source_ext,
                          'imfeat/_histogram/histogram_aux.c'],
                         extra_compile_args=['-I', np.get_include()],
                         include_dirs=['imfeat']),
               Extension("_imfeat_dedupe",
                         ["imfeat/_dedupe/dedupe" + source_ext,
                          'imfeat/_dedupe/dedupe_aux.c'],
                         extra_compile_args=['-I', np.get_include()],
                         include_dirs=['imfeat']),
               Extension("_imfeat_autocorrelogram",
                         ["imfeat/_autocorrelogram/autocorrelogram" + source_ext,
                          'imfeat/_autocorrelogram/Correlogram.cpp'],
                         extra_compile_args=['-I', np.get_include()]),
               Extension("_imfeat_rhog",
                         ["imfeat/_rhog/rhog" + source_ext,
                          'imfeat/_rhog/rhog_aux.c'],
                         extra_compile_args=['-I', np.get_include()]),
               Extension("_imfeat_block_generator",
                         ["imfeat/block_generator" + source_ext],
                         extra_compile_args=['-I', np.get_include()]),
               Extension("_imfeat_bovw",
                         ["imfeat/_bovw/bovw" + source_ext, 'imfeat/_bovw/bovw_aux.c'],
                         extra_compile_args=['-I', np.get_include()]),
               Extension("_imfeat_pyramid_histogram",
                         ["imfeat/_pyramid_histogram/pyramid_histogram" + source_ext, 'imfeat/_pyramid_histogram/pyramid_histogram_aux.c'],
                         extra_compile_args=['-I', np.get_include()]),
               Extension("_imfeat_moments",
                         ["imfeat/moments" + source_ext],
                         extra_compile_args=['-I', np.get_include()]),
               Extension("_imfeat_hog_latent",
                         ["imfeat/_hog_latent/hog_latent" + source_ext,
                          'imfeat/_hog_latent/features.cc',
                          'imfeat/_hog_latent/resize.cc'],
                         extra_compile_args=['-I', np.get_include()]),
               Extension("_imfeat_lbp",
                         ["imfeat/_lbp/lbp" + source_ext,
                          'imfeat/_lbp/lbp_aux.c'],
                         extra_compile_args=['-I', np.get_include()]),
               Extension("_imfeat_gist",
                         ["imfeat/_gist/gist_cython" + source_ext,
                          'imfeat/_gist/gist.c',
                          'imfeat/_gist/gist_wrapper.c',
                          'imfeat/_gist/standalone_image.c'],
                         extra_compile_args=['-I', np.get_include(), '-D', 'USE_GIST',
                                             '-D', 'STANDALONE_GIST'],
                         extra_link_args=['-l', 'fftw3f'])]

for e in ext_modules:
    e.pyrex_directives = { "embedsignature": True }


setup(
        name='imfeat',
        cmdclass=cmdclass,
        version='.01',
        packages=[
            'imfeat', 'imfeat._rhog_dalal',
            'imfeat._faces', 'imfeat._object_bank',
            'imfeat._texton', 'imfeat._color_naming'
        ],
        package_data={
            'imfeat._faces': ['data/*'], 'imfeat._color_naming': ['data/*'],
            'imfeat._object_bank': ['data/OBmain', 'data/models/*']
        },
        ext_modules=ext_modules)
