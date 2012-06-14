import os, sys, re
import shutil
from ost import settings
from optparse import OptionParser
import subprocess

if len(sys.argv)==2:
  root_dir=sys.argv[1]
else:
  root_dir='.'

def _CheckCall(cmd, shell):
  r = subprocess.call(cmd, shell=True)
  if r != 0:
    sys.stderr.write("Command '%s' returned non-zero exit status %d\n"%(cmd, r))
    sys.exit(-1)

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


pattern = re.compile('[a-zA-Z0-9_//]+\.png|[a-zA-Z0-9_//]+\.jpg')
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
    print 'cp %s %s' % (in_name, out_name)
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

opt_str='' 
if opts.quiet:
  opt_str=' -Q '

for sub_dir in ('modules',):
  os.path.walk(sub_dir, _CollectRstDocs, 'doc/source')
sphinx_bin=settings.Locate(['sphinx-build', 'sphinx-build-2.6'])

if opts.html:
  cmd='%s %s -b html -c %s %s %s' % (sphinx_bin, opt_str, 
                                     'doc/conf', 'doc/source', 'doc/build/html')
  print cmd
  _CheckCall(cmd, shell=True)

if opts.doctest:
  cmd='%s %s -b doctest -c %s %s %s' % (sphinx_bin, opt_str, 
                                        'doc/conf', 'doc/source', 
                                        'doc/build/doctest')
  _CheckCall(cmd, shell=True)
if opts.build_json:
  cmd='%s %s -b json -c %s %s %s' % (sphinx_bin, opt_str, 'doc/conf', 
                                     'doc/source', 'doc/build/json')
  _CheckCall(cmd, shell=True)
if opts.linkcheck:
  cmd='%s %s -b linkcheck -c %s %s %s' % (sphinx_bin, opt_str, 'doc/conf', 
                                          'doc/source', 'doc/build/check')
  _CheckCall(cmd, shell=True)
