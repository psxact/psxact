import os, fnmatch

env = Environment()
env['CCFLAGS'] = [
  '-flto',
  '-std=c++17',
  '-Wall',
  '-Wextra',
  '-I', 'src'
]

env['LINKFLAGS'] = [
  '-flto'
]

debug = ARGUMENTS.get('debug', 0)

if int(debug):
  env.Append(CCFLAGS = '-g')
else:
  env.Append(CCFLAGS = '-O3')

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
