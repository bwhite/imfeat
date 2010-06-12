import distutils.sysconfig
env = Environment()
env.Replace(CXX = 'g++')
env.Append(CCFLAGS =  '-O3 -Wall')

lib_path = 'vitrieve_algorithms/lib/'
va = 'vitrieve_algorithms/'
vaf = va + 'features/'
env.SharedLibrary(lib_path + 'hamming_embedding', [va + 'hamming_embedding.c'])
env.SharedLibrary(lib_path + 'nearest_neighbor', [va + 'nearest_neighbor.c'])
env.SharedLibrary(lib_path + 'knearest_neighbor', [va + 'knearest_neighbor.c'])

exp = ['env']
SConscript(vaf + 'histogram_joint/SConstruct', exp)
SConscript(vaf + 'moments/SConstruct', exp)
SConscript(vaf + 'surf/SConstruct', exp)
