import os, fnmatch

env = Environment()
env['CCFLAGS'] = [
  '-std=c++17',
  '-Wall',
  '-Wextra',
  '-I', 'lib'
]

env['LINKFLAGS'] = []

debug = ARGUMENTS.get('debug', 0)

if int(debug):
  env.Append(CCFLAGS=['-pg', '-rdynamic', '-no-pie'])
  env.Append(LINKFLAGS=['-pg', '-rdynamic', '-no-pie'])
else:
  env.Append(CCFLAGS=['-flto', '-O3'])
  env.Append(LINKFLAGS=['-flto'])

def build_obj(file):
  return env.Object('build/{}.o'.format(file), file)

def build_objs(folder):
  matches = []
  for root, _, filenames in os.walk(folder):
    for filename in fnmatch.filter(filenames, '*.cpp'):
      matches.append(build_obj(os.path.join(root, filename)))
  return matches

lib = \
  env.StaticLibrary(target='build/psxact',
                    source=build_objs('lib'))

app = \
  env.Program(target='build/psxact',
              source=build_objs('src'),
              LIBS=[lib, 'SDL2'])

test = \
  env.Program(target='build/psxact-tests',
              source=build_objs('test'),
              LINKFLAGS=['-pthread'],
              LIBS=[lib, 'gtest', 'gtest_main'])
