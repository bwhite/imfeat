import distutils.sysconfig
env = Environment()
env.Replace(CXX = 'g++')
env.Append(CCFLAGS =  '-O3 -Wall')

exp = ['env']
SConscript('imfeat/moments/SConstruct', exp)
SConscript('imfeat/histogram_joint/SConstruct', exp)
SConscript('imfeat/histogram/SConstruct', exp)
