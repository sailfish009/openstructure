import os
import subprocess
import shutil
import sys
import glob

def _lib_name(component):
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

def collect_deps(stage_dir, components, binaries, site_packages, 
                 site_packages_dir):
  """
  Collect the dependencies for the given components and returns a list of 
  frameworks/libraries that the component depends on.
  """
  pool=set()
  for component in components:
    lib_name=os.path.abspath(os.path.join(stage_dir, 'lib', 
                                          _lib_name(component)))  
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
  for site_package in site_packages:
    full_path=os.path.join(site_packages_dir, site_package)
    if not os.path.exists(full_path):
      print 'WARNING', site_package, 'does not exists'
      continue
    if os.path.isdir(full_path):
      for so_file in glob.glob(os.path.join(full_path, '*.so')):
        _deps_for_lib(so_file, pool)
  return pool

BINARIES=['gosty', 'chemdict_tool']
COMPONENTS=['mol', 'geom', 'conop', 'gfx', 'gui', 'seq_alg', 'seq', 
            'img', 'img_alg', 'qa', 'info', 'io', 'db', 'base']
SCRIPTS=['dng', 'ost']
CHANGE_ID_RPATH='install_name_tool -id @rpath/%s %s'   
CHANGE_ID='install_name_tool -id @executable_path/%s %s'   
CHANGE_LOAD_CMD_RPATH='install_name_tool -change %s @rpath/%s %s'
CHANGE_LOAD_CMD='install_name_tool -change %s @executable_path/%s %s'
ADD_RPATH='install_name_tool -add_rpath %s %s 2> /dev/null'
SITE_PACKAGES=['sip.so', 'sipconfig.py', 'sipdistutils.py', 'PyQt4']
REMOVE_HEADERS='rm -rf `find %s/lib -type d -name Headers`'
REMOVE_CURRENT='rm -rf `find %s/lib -type d -name Current`'
# collect libs of non-standard libraries/frameworks we depend on

def copy_binaries(stage_dir, outdir, binary_names, scripts, use_rpath, 
                  append_bin=True):

  exe_path=os.path.abspath(os.path.join(outdir, 'bin'))
  for binary_name in binary_names:
    if append_bin:
      bin_name=os.path.join(stage_dir, 'bin', binary_name)
    else:
      bin_name=os.path.join(stage_dir, binary_name)
    if not os.path.exists(bin_name):
      print 'WARNING:', binary_name, 'does not exist'
      continue
    dst_name=os.path.join(outdir, 'bin', os.path.basename(bin_name))
    shutil.copy(bin_name, dst_name)
    update_load_commands(dst_name, True, use_rpath, exe_path)
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

def change_id(id, lib, use_rpath):
  os.chmod(lib, 0666)
  if use_rpath:
    os.system(CHANGE_ID_RRPATH % (id,lib))
  else:
    os.system(CHANGE_ID % (id,lib))
  os.chmod(lib, 0444)

def update_load_commands(lib, exe, use_rpath, exe_path):
  direct_deps=set()
  _deps_for_lib(lib, direct_deps, recursive=False)
  os.chmod(lib, 0666)
  for direct_dep in direct_deps:
    if direct_dep.endswith('.dylib'):
      if use_rpath:
        new_name=os.path.basename(direct_dep)
        os.system(CHANGE_LOAD_CMD_RPATH % (direct_dep, new_name, lib))
      else:
        new_name=os.path.join('../lib', os.path.basename(direct_dep))
        os.system(CHANGE_LOAD_CMD % (direct_dep, new_name, lib))
    else:
      assert direct_dep.find('.framework/')>=0
      if use_rpath:
        framework_path, rel_path=split_framework_components(direct_dep)
        framework_name=os.path.basename(framework_path)
        new_name=os.path.join(framework_name, rel_path)
        os.system(CHANGE_LOAD_CMD_RPATH % (direct_dep, new_name, lib))
      else:
        framework_path, rel_path=split_framework_components(direct_dep)
        framework_name=os.path.basename(framework_path)      
        new_name=os.path.join('../lib', framework_name, rel_path)
        os.system(CHANGE_LOAD_CMD % (direct_dep, new_name, lib))
  if use_rpath:
    os.system(ADD_RPATH % ('.', lib))
  if exe:
    os.chmod(lib, 0555)
  else:
    os.chmod(lib, 0444)

def copy_deps(dependencies, outdir, use_rpath):
  exe_path=os.path.join(outdir, 'bin')
  for dep in dependencies:
    if dep.endswith('.dylib'):
      dst_name=os.path.join(outdir, 'lib', os.path.basename(dep))
      shutil.copy(dep, dst_name)
      if use_rpath:
        change_id(os.path.basename(dep), dst_name, use_rpath)
      else:
        change_id('../lib/%s' % os.path.basename(dep), dst_name, use_rpath)
      update_load_commands(dst_name, False, use_rpath, exe_path)
    else:
      assert dep.find('.framework/')>=0
      framework_path, rel_path=split_framework_components(dep)
      framework_name=os.path.basename(framework_path)
      dst_name=os.path.join(outdir, 'lib', framework_name)
      shutil.copytree(framework_path, dst_name)
      if use_rpath:
        change_id(os.path.join(dst_name, rel_path), 
                  os.path.join(dst_name, rel_path), use_rpath)
      else:
        change_id(os.path.join('../lib', framework_name, rel_path), 
                  os.path.join(dst_name, rel_path), use_rpath)
      update_load_commands(os.path.join(dst_name, rel_path), False, 
                           use_rpath, exe_path)

def update_pymod_shared_objects(args, path, files):
  lib_path, use_rpath=args
  exe_path=os.path.abspath(os.path.join(lib_path, '../bin'))
  for f in files:
    if not os.path.exists(os.path.join(path, f)):
      continue
    base, ext=os.path.splitext(f)
    if  ext=='.so':
      path_to_lib_path=os.path.relpath(lib_path, path)
      abs_name=os.path.join(path, f)
      os.system(ADD_RPATH % (path_to_lib_path, abs_name))
      update_load_commands(abs_name, False, use_rpath, exe_path)
    elif ext in ('.pyc', '.pyo'):
      os.unlink(os.path.join(path, f))

def get_site_package_dir():
  """
  Get site-package directory of this python installation. This assumes 
  that ost was linked against the same version of Python
  """
  for p in sys.path:
    pattern='/site-packages'
    index=p.find(pattern)
    if index>=0:
      return p[:index+len(pattern)]
  raise RuntimeError("Couldn't determine site-packages location")

def get_python_home():
  """
  Derive Python home by looking at the location of the os module
  """
  return os.path.dirname(sys.modules['os'].__file__)

def check_install_name_tool_capabilities():
  """
  Find out whether install_name_tool supports the add_rpath option.
  """
  inst_name_tool=subprocess.Popen('install_name_tool', shell=True, 
                                  stderr=subprocess.PIPE)
  output=inst_name_tool.communicate()[1]
  return output.find('-add_rpath')!=-1

def make_standalone(stage_dir, outdir, no_includes, force_no_rpath=False,
                    macports_workaround=False):
  site_packages=get_site_package_dir()                    
  # figure out if install_name_tool supports the -add_rpath option.
  use_rpath=True
  if not check_install_name_tool_capabilities():
    print "install_name_tool doesn't support the -add_rpath option."
    print "I will fallback to the more arcane @executable_path"
    use_rpath=False
  elif force_no_rpath:
    print "I will use the arcane @executable_path"
    use_rpath=False

  if os.path.exists(outdir):
    shutil.rmtree(outdir)
  os.system('mkdir -p "%s"' % outdir)
  os.system('mkdir -p "%s/lib"' % outdir)
  os.system('mkdir -p "%s/bin"' % outdir)
  print 'copying shared datafiles'
  shutil.copytree(os.path.join(stage_dir, 'share'), 
                  os.path.join(outdir, 'share'))
  print 'collecting dependencies'
  deps=collect_deps(stage_dir, COMPONENTS, BINARIES, SITE_PACKAGES, 
                    site_packages)
  print 'copying dependencies'
  copy_deps(deps, outdir, use_rpath)
  print 'copying binaries'
  copy_binaries(stage_dir, outdir, BINARIES, SCRIPTS, use_rpath)
  print 'copying pymod'
  shutil.copytree(os.path.join(stage_dir, 'lib/openstructure'), 
                  os.path.join(outdir, 'lib/openstructure'))
  copied_py_framework=False
  non_std_python=False
  if os.path.exists(os.path.join(outdir, 'lib/Python.framework')):
    framework_path=os.path.join(outdir, 'lib/Python.framework')
    nonstd_python=True
    copied_py_framework=True
  if glob.glob(os.path.join(outdir, 'lib', 'libpython*'))>0:
    non_std_python=True
  if non_std_python:
    print 'looks like we are using a non-standard python.'
    python_home=get_python_home()    
    if not copied_py_framework:
      print 'also copying python modules from %s' % python_home
      modules_dst=os.path.join(outdir, 'lib', os.path.basename(python_home))
      shutil.copytree(python_home, modules_dst)
      shutil.rmtree(os.path.join(modules_dst, 'site-packages'))
      copy_binaries(os.path.join(python_home, '../..'), outdir, 
                    ['python'], [], use_rpath)
      python_bin=os.path.abspath(os.path.join(python_home, '../../bin/python'))
    else:
      # For MacPorts it's even more involved. Python is not executed directly 
      # but rather uses a wrapper executable that calls the actual python exe.
      # We have to include that one into the bundle.
      if macports_workaround:
        path_to_app='../../Resources/Python.app/Contents/MacOS/'
        exe_path=os.path.join(python_home, path_to_app)
        copy_binaries(exe_path, outdir, ['python'], [], 
                      use_rpath, append_bin=False)
        python_bin=os.path.join('/opt/local/bin/python')
      else:
        copy_binaries(os.path.join(python_home, '../..'), outdir, 
                      ['python'], [], use_rpath)
        python_bin=os.path.abspath(os.path.join(python_home, '../../bin/python'))
      # remove all versions but the one we are using
      version_string=sys.version[0:3]
      prefix, postfix=split_framework_components(python_home)
      site_packages_dir=os.path.join(outdir, 'lib', 'Python.framework', 
                                 postfix, 'site-packages')
      shutil.rmtree(site_packages_dir)
      for directory in glob.glob(os.path.join(framework_path, 'Versions/*')):
        if os.path.basename(directory)!=version_string:
          shutil.rmtree(directory)
    # replace the python executable
    ost_script=os.path.join(outdir, 'bin', 'ost')
    os.chmod(ost_script, 0666)
    script=''.join(open(ost_script, 'r').readlines())
    script=script.replace(python_bin, '$BIN_DIR/python')
    open(ost_script, 'w').write(script)
    os.chmod(ost_script, 0555)
  elif use_rpath==False:
    print 'BIG FAT WARNING: Creation bundle with @executable_path and default'
    print 'Python might not work. Test carefully before deploying.'

  if no_includes:
    os.system(REMOVE_HEADERS % outdir)
    os.system(REMOVE_CURRENT % outdir)  
  print 'copying site-packages'
  for sp in SITE_PACKAGES:
    src=os.path.join(site_packages, sp)  
    if os.path.isdir(src):
      shutil.copytree(src, os.path.join(outdir, 'lib/openstructure', sp))
    else:
      shutil.copy(src, os.path.join(outdir, 'lib/openstructure', sp))
  print 'updating link commands of python shared objects'
  os.path.walk(os.path.join(outdir, 'lib'), 
               update_pymod_shared_objects, 
               (os.path.join(outdir, 'lib'), use_rpath))
