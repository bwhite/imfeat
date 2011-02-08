import distutils.sysconfig
env = Environment()
env.Replace(CXX = 'g++')
env.Append(CCFLAGS =  '-O3 -Wall -fPIC')

exp = ['env']
SConscript('imfeat/_moments/SConstruct', exp)
SConscript('imfeat/_rhog/SConstruct', exp)
SConscript('imfeat/_histogram_joint/SConstruct', exp)
SConscript('imfeat/_histogram/SConstruct', exp)
SConscript('imfeat/_surf/SConstruct', exp)