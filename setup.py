from distutils.core import setup

setup(name='imfeat',
      version='.01',
      packages=['imfeat', 'imfeat.moments', 'imfeat.histogram_joint', 'imfeat.histogram'],
      package_data = {'imfeat.moments': ['lib/*.so'], 'imfeat.histogram_joint': ['lib/*.so'], 'imfeat.histogram': ['lib/*.so']}
      )
