'''Documentation build on sphinx - please run from root dir by calling ost doc/make.py'''

import os, sys, re
import shutil
from ost import settings
from optparse import OptionParser
import subprocess
import sphinx.cmd.build

if len(sys.argv)==2:
  root_dir=sys.argv[1]
else:
  root_dir='.'

def _OutputPath(inpath, outdir):
  parts=inpath.split(os.path.sep)
  filtered_parts=[outdir]
  index=parts.index('modules')
  for part in parts[index+1:]:
    if part!='doc':
      filtered_parts.append(part)
  return os.path.join(*filtered_parts)

def _RequireCopy(in_name, out_name):
  if not os.path.exists(out_name):
    return True
  if os.path.getmtime(in_name)>os.path.getmtime(out_name):
    return True
  return False


pattern = re.compile(r'\.\.\s+image\:\:\s+([a-zA-Z0-9_\-//]+\.png|[a-zA-Z0-9_\-//]+\.jpg)')
def _CheckImage(in_name):
  file = open(in_name, "r")
  text = file.read()
  picture_list = pattern.findall(text)
  file.close()
  return picture_list

def _CreateAndCopy(in_name, outdir):
  out_name=_OutputPath(in_name, outdir)
  out_dir=os.path.dirname(out_name)
  if not os.path.exists(out_dir):
    _MkDir(out_dir)
  if _RequireCopy(in_name, out_name):
    print('cp %s %s' % (in_name, out_name))
    os.system('cp %s %s' % (in_name, out_name))

def _MkDir(dirname):
  """
  Recursively create directories if they don't exist
  """
  parts=dirname.split(os.path.sep)
  for i, d in enumerate(parts):
    n=os.path.join(*parts[:1+i])
    if not os.path.exists(n):
      os.mkdir(n)

def _CollectRstDocs(outdir, dirname, fnames):
  for fname in fnames:
    if fname.endswith('.rst'):
      in_name=os.path.join(dirname, fname)
      _CreateAndCopy(in_name,outdir)
      img_list = _CheckImage(in_name)
      for img in img_list:
        img_name = os.path.join(dirname,img)
        _CreateAndCopy(img_name, outdir)

def ParseArgs():
  parser = OptionParser("usage: ost make.py [options] ")
  parser.add_option("-l","--linkcheck", action="store_true", default=False, dest="linkcheck", help="validate all external links")
  parser.add_option("-b", "--build-html", action="store_true",
                    default=False, dest="html", help="build html documentation")
  parser.add_option('-j', '--build-json', action='store_true', default=False)
  parser.add_option("-d", "--doctest", action="store_true", default=False, dest="doctest", help="run all test")
  parser.add_option("-q", "--quiet", action="store_true", default=False, dest="quiet", help="run all test")
  return parser.parse_args()

opts, args=ParseArgs()
if not opts.html and\
   not opts.linkcheck and\
   not opts.doctest:
     opts.html=True

extra_opts=[]
if opts.quiet:
  extra_opts=['-Q']

for sub_dir in ('modules',):
  for directory, dirnames, filenames in os.walk(sub_dir):
    _CollectRstDocs('doc/source', directory, filenames)

if opts.html:
  sphinx.cmd.build.main(extra_opts + ['-b', 'html', '-c', 'doc/conf', 'doc/source', 'doc/build/html'])

if opts.doctest:
  sphinx.cmd.build.main(extra_opts + ['-b', 'doctest', '-c', 'doc/conf', 'doc/source', 'doc/build/html'])

if opts.build_json:
  sphinx.cmd.build.main(extra_opts + ['-b', 'json', '-c', 'doc/conf', 'doc/source', 'doc/build/html'])

if opts.linkcheck:
  sphinx.cmd.build.main(extra_opts + ['-b', 'linkcheck', '-c', 'doc/conf', 'doc/source', 'doc/build/html'])

