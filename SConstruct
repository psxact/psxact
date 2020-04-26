import os, fnmatch

env = Environment()
env['CCFLAGS'] = [
  '-std=c++17',
  '-Wall',
  '-Wextra',
  '-I', 'src'
]

env['LINKFLAGS'] = []

debug = ARGUMENTS.get('debug', 0)

if int(debug):
  env.Append(CCFLAGS=['-pg', '-no-pie'])
  env.Append(LINKFLAGS=['-pg', '-no-pie'])
else:
  env.Append(CCFLAGS=['-O3', '-flto'])
  env.Append(LINKFLAGS=['-flto'])

def build_obj(file):
  return env.Object('build/{}.o'.format(file), file)

def build_objs():
  matches = []
  for root, _, filenames in os.walk('src'):
    for filename in fnmatch.filter(filenames, '*.cpp'):
      matches.append(build_obj(os.path.join(root, filename)))
  return matches

env.Program(target='build/psxact',
            source=build_objs(),
            LIBS=['SDL2'])
