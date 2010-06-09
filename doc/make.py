import os, sys
import shutil
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
      out_name=_OutputPath(in_name, outdir)
      out_dir=os.path.dirname(out_name)
      if not os.path.exists(out_dir):
        _MkDir(out_dir)
      if _RequireCopy(in_name, out_name):
        print 'cp %s %s' % (in_name, out_name)
        os.system('cp %s %s' % (in_name, out_name))
      
for sub_dir in ('modules',):
  os.path.walk(sub_dir, _CollectRstDocs, 'doc/source')
os.system('sphinx-build -b html -c %s %s %s' % ('doc/conf', 'doc/source', 
                                                'doc/build'))