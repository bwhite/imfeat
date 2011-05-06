from distutils.core import setup

setup(name='imfeat',
      version='.01',
      packages=['imfeat', 'imfeat._moments', 'imfeat._histogram_joint',
                'imfeat._histogram', 'imfeat._rhog', 'imfeat._rhog_dalal',
                'imfeat._surf', 'imfeat._faces'],
      package_data={'imfeat._moments': ['lib/*.so'],
                    'imfeat._rhog': ['lib/*.so'],
                    'imfeat._surf': ['lib/*.so'],
                    'imfeat._histogram_joint': ['lib/*.so'],
                    'imfeat._histogram': ['lib/*.so'],
                    'imfeat._faces': ['data/*']})
