from distutils.core import setup

setup(name='imfeat',
      version='.01',
      packages=['imfeat', 'imfeat.moments', 'imfeat.histogram_joint',
                'imfeat.histogram', 'imfeat.rhog', 'imfeat.rhog_dalal',
                'imfeat.surf'],
      package_data={'imfeat.moments': ['lib/*.so'],
                    'imfeat.rhog': ['lib/*.so'],
                    'imfeat.surf': ['lib/*.so'],
                    'imfeat.histogram_joint': ['lib/*.so'],
                    'imfeat.histogram': ['lib/*.so']})
