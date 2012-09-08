#!/usr/bin/env python
import cStringIO
import ninja_syntax
import optparse
import os
import sys

SCRIPT_DIR = os.path.dirname(__file__)


def PrefixPath(path, seq):
  return [os.path.join(path, x) for x in seq]


def ReadListFromFile(path):
  return map(str.strip, open(path, 'r').readlines())


def Prefix(prefix, items):
  return ' '.join(prefix + x for x in items.split())


def Join(*args):
  return ' '.join(args)


def SourceToObj(source, bits):
  return os.path.join('out', '%s.%s.o' % (os.path.splitext(source)[0], bits))


class Writer(ninja_syntax.Writer):
  def __init__(self, stream):
    ninja_syntax.Writer.__init__(self, stream)
    self.pyscript_imports = {}

  def PyScriptRule(self, name, command, imports=None):
    script = 'tools/{0}.py'.format(name)
    self.newline()
    self.rule(name,
        command=command.format(script=script),
        description='{0} $out'.format(name.upper()))
    if imports is None:
      imports = []
    if isinstance(imports, str):
      imports = [imports]
    self.pyscript_imports[name] = imports

  def PyScriptBuild(self, name, outs, ins, implicits=None, **kwargs):
    script = 'tools/{0}.py'.format(name)
    if implicits is None:
      implicits = []
    if isinstance(implicits, str):
      implicits = [implicits]
    self.build(outs, name, ins,
        implicit=[script] + self.pyscript_imports[name] + implicits,
        variables=kwargs)

  def PyScriptRuleBuild(self, name, command, outs, ins, implicits=None,
      imports=None, **kwargs):
    self.PyScriptRule(name, command, imports=imports)
    self.PyScriptBuild(name, outs, ins, implicits, **kwargs)


SOURCE_FILES = [
  'source/background.cpp',
  'source/color.cpp',
  'source/misc.cpp',
  'source/object.cpp',
  'source/particle.cpp',
  'source/player.cpp',
  'source/point.cpp',
  'source/region.cpp',
  'source/segment.cpp',
  'source/simfun.cpp',
  'source/simulation.cpp',
  'source/tilemap.cpp',
  'source/tiles.cpp',
  'source/vector.cpp',
  'source/walls.cpp',
  'source/wallset.cpp',
]
DATA_FILES = []
MAKE_NINJA = './build/make_ninja.py'


def Code(w):
  w.newline()
  w.rule('cc',
      command='$cc $cflags -MMD -MF $out.d -c $in -o $out',
      depfile='$out.d',
      description='CC $out')
  w.rule('link',
      command='$cc $in $ldflags -o $out',
      description='LINK $out')

  libs = Prefix('-l', '''ppapi_cpp ppapi''')

  flags = '-g'
  defines = Prefix('-D', '_GNU_SOURCE=1 _REENTRANT')
  includes = Prefix('-I', 'include')

  for bits, flavor in (('32', 'i686-nacl'), ('64', 'x86_64-nacl')):
    includes = Join(includes, Prefix('-I',
        '$usr_include{bits}/SDL $usr_include{bits}'.format(**vars())))
    w.variable('usr_include' + bits,
        '$toolchain_dir/{flavor}/usr/include'.format(**vars()))
    w.variable('usr_lib' + bits,
        '$toolchain_dir/{flavor}/usr/lib'.format(**vars()))

    w.variable('cflags' + bits, '{flags} {defines} {includes}'.format(**vars()))
    w.variable('ldflags' + bits,
        '-L$usr_lib{bits} {libs}'.format(**vars()))
    w.variable('cc' + bits, '$toolchain_dir/bin/{flavor}-g++'.format(**vars()))

    sources = SOURCE_FILES
    objs = [SourceToObj(x, bits) for x in sources]
    for source, obj in zip(sources, objs):
      w.build(obj, 'cc', source,
          variables={'cflags': '$cflags' + bits, 'cc': '$cc' + bits})

    w.build('out/simfun.{bits}.nexe'.format(**vars()), 'link', objs,
        variables={'ldflags': '$ldflags' + bits,
                   'cc': '$cc' + bits})


def Data(w):
  pass


def main():
  parser = optparse.OptionParser()
  options, args = parser.parse_args()

  out_filename = os.path.join(os.path.dirname(__file__), '../build.ninja')
  f = cStringIO.StringIO()
  w = Writer(f)

  w.rule('configure', command = MAKE_NINJA, generator=1)
  w.build('build.ninja', 'configure', implicit=[MAKE_NINJA])
  w.variable('nacl_sdk_usr', '/dev/naclsdk/nacl_sdk/pepper_21')
  w.variable('toolchain_dir', '$nacl_sdk_usr/toolchain/win_x86_newlib')

  Code(w)
  Data(w)

  # Don't write build.ninja until everything succeeds
  open(out_filename, 'w').write(f.getvalue())



if __name__ == '__main__':
  sys.exit(main())
