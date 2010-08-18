from distutils.core import setup

setup(name='imfeat',
      version='.01',
      packages=['imfeat', 'imfeat.moments', 'imfeat.histogram_joint', 'imfeat.histogram', 'imfeat.rhog', 'imfeat.rhog_dalal'],
      package_data = {'imfeat.moments': ['lib/*.so'], 'imfeat.rhog': ['lib/*.so'], 'imfeat.histogram_joint': ['lib/*.so'], 'imfeat.histogram': ['lib/*.so'], 'imfeat.rhog_dalal': ['*']}
      )
