from distutils.core import setup

setup(name='imfeat',
      version='.01',
      packages=[_va, _vaf, _vaf + '.moments', _vaf + '.histogram_joint', _vaf + '.surf'],
      package_data = {_va : ['lib/*'], _vaf + '.moments': ['lib/*'], _vaf + '.histogram_joint': ['lib/*'], _vaf + '.surf': ['lib/*']}
      )
