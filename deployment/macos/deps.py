import os
import subprocess
import shutil
import sys
def _LibName(component):
  return 'libost_%s.dylib' % component


def _deps_for_lib(lib, pool, recursive=True):
  if lib in pool:
    return
  otool=subprocess.Popen(['otool', '-L', lib], stdout=subprocess.PIPE)
  output=otool.communicate()[0]
  lines=output.split('\n')[1:]
  for line in lines:
    d=line.split(' ')[0].strip()
    if len(d)>0:
      if d==lib:
        continue
      if d not in pool:
        if d.startswith('/System') or d.startswith('/usr/lib'):
          continue
        if recursive:
          _deps_for_lib(d, pool)
        pool.add(d)        
  return

def collect_deps(stage_dir, components, binaries):
  """
  Collect the dependencies for the given components and returns a list of 
  frameworks/libraries that the component depends on.
  """
  pool=set()
  for component in components:
    lib_name=os.path.abspath(os.path.join(stage_dir, 'lib', 
                                          _LibName(component)))  
    if not os.path.exists(lib_name):
      print 'WARNING:', lib_name, 'does not exist'
    if lib_name not in pool:
      _deps_for_lib(lib_name, pool)
      pool.add(lib_name)    
  for bin in binaries:  
    bin_name=os.path.abspath(os.path.join(stage_dir, 'bin', 
                                          bin))  
    if not os.path.exists(bin_name):
      print 'WARNING:', bin_name, 'does not exist'
      continue
    if bin_name not in pool:
      _deps_for_lib(bin_name, pool)
  return pool

BINARIES=['gosty', 'chemdict_tool']
COMPONENTS=['mol', 'geom', 'conop', 'gfx', 'gui', 'seq_alg', 'seq', 
            'img', 'img_alg', 'qa', 'info', 'io', 'db', 'base']
SCRIPTS=['dng', 'ost']
CHANGE_ID='install_name_tool -id @rpath/%s %s'   
CHANGE_LOAD_CMD='install_name_tool -change %s @rpath/%s %s'
ADD_RPATH='install_name_tool -add_rpath %s %s 2> /dev/null'
SITE_PACKAGES=['sip.so', 'sipconfig.py', 'sipdistutils.py', 'PyQt4']
REMOVE_HEADERS='rm -rf `find %s/lib -type d -name Headers`'
REMOVE_CURRENT='rm -rf `find %s/lib -type d -name Current`'
# collect libs of non-standard libraries/frameworks we depend on

def copy_binaries(stage_dir, outdir, binary_names, scripts):
  for binary_name in binary_names:
    bin_name=os.path.abspath(os.path.join(stage_dir, 'bin', binary_name))  
    if not os.path.exists(bin_name):
      print 'WARNING:', binary_name, 'does not exist'
      continue
    dst_name=os.path.join(outdir, 'bin', os.path.basename(bin_name))
    shutil.copy(bin_name, dst_name)
    update_load_commands(dst_name)
    os.system(ADD_RPATH % ('../lib', dst_name))
  for script in scripts:
    shutil.copy(os.path.join(stage_dir, 'bin', script), 
                os.path.join(outdir,'bin', script))

def split_framework_components(abs_path):
    """
    Splits the path pointing to a dynamic library within a framework
    
    '/System/Frameworks/A.framework/Versions/4/A' =>
    ['/System/Frameworks/A.framework', 'Versions/4/A']
    """
    parts=abs_path.split('/')
    for i, s in enumerate(parts):
      if s.endswith('.framework'):
        lead=os.path.join('/', *parts[:i+1])
        trail=os.path.join(*parts[i+1:])
        return lead, trail

def update_load_commands(lib):
  direct_deps=set()
  _deps_for_lib(lib, direct_deps, recursive=False)
  for direct_dep in direct_deps:
    if direct_dep.endswith('.dylib'):
      new_name=os.path.basename(direct_dep)
      os.system(CHANGE_LOAD_CMD % (direct_dep, new_name, lib))
    else:
      assert direct_dep.find('.framework/')>=0
      framework_path, rel_path=split_framework_components(direct_dep)
      framework_name=os.path.basename(framework_path)      
      dst_name=os.path.join(framework_name, rel_path)
      new_name=os.path.join(framework_name, rel_path)
      os.system(CHANGE_LOAD_CMD % (direct_dep, new_name, lib))
  os.system(ADD_RPATH % ('.', lib))

def copy_deps(dependencies, outdir):
  for dep in dependencies:
    if dep.endswith('.dylib'):
      dst_name=os.path.join(outdir, 'lib', os.path.basename(dep))
      shutil.copy(dep, dst_name)
      os.system(CHANGE_ID % (os.path.basename(dep), dst_name))
      update_load_commands(dst_name)
    else:
      assert dep.find('.framework/')>=0
      framework_path, rel_path=split_framework_components(dep)
      framework_name=os.path.basename(framework_path)
      dst_name=os.path.join(outdir, 'lib', framework_name)
      shutil.copytree(framework_path, dst_name)
      os.system(CHANGE_ID % (os.path.join(dst_name, rel_path), 
                             os.path.join(dst_name, rel_path)))
      update_load_commands(os.path.join(dst_name, rel_path))

def update_pymod_shared_objects(lib_path, path, files):
  for f in files:
    if os.path.splitext(f)[1] == '.so':
      path_to_lib_path=os.path.relpath(lib_path, path)
      abs_name=os.path.join(path, f)
      os.system(ADD_RPATH % (path_to_lib_path, abs_name))
      update_load_commands(abs_name)

def get_site_package_dir():
  """
  Get site-package directory of this python installation. This assumes 
  that ost was linked against the same version of Python
  """
  for p in sys.path:
    pattern='/site-packages/'
    index=p.find(pattern)
    if index>=0:
      return p[:index+len(pattern)]
  raise RuntimeError("Couldn't determine site-packages location")

def make_standalone(stage_dir, outdir, no_includes):
  if os.path.exists(outdir):
    shutil.rmtree(outdir)
  os.system('mkdir -p "%s"' % outdir)
  os.system('mkdir -p "%s/lib"' % outdir)
  os.system('mkdir -p "%s/bin"' % outdir)
  print 'copying shared datafiles'
  shutil.copytree(os.path.join(stage_dir, 'share'), 
                  os.path.join(outdir, 'share'))
  print 'collecting dependencies'
  deps=collect_deps(stage_dir, COMPONENTS, BINARIES)
  print 'copying dependencies'
  copy_deps(deps, outdir)
  print 'copying binaries'
  copy_binaries(stage_dir, outdir, BINARIES, SCRIPTS)
  print 'copying pymod'
  shutil.copytree(os.path.join(stage_dir, 'lib/openstructure'), 
                  os.path.join(outdir, 'lib/openstructure'))
  if no_includes:
    os.system(REMOVE_HEADERS % outdir)
    os.system(REMOVE_CURRENT % outdir)  
  site_packages=get_site_package_dir()
  print 'copying site-packages'
  for sp in SITE_PACKAGES:
    src=os.path.join(site_packages, sp)  
    if os.path.isdir(src):
      shutil.copytree(src, os.path.join(outdir, 'lib/openstructure', sp))
    else:
      shutil.copy(src, os.path.join(outdir, 'lib/openstructure', sp))
  print 'updating link commands of python shared objects'
  os.path.walk(os.path.join(outdir, 'lib', 'openstructure'), 
               update_pymod_shared_objects, 
               os.path.join(outdir, 'lib'))
