import os,os.path
import shutil
import sys
import tarfile
import subprocess
import re

def get_libdir():
  machine=os.uname()[4]
  if machine.find('x86_64') !=-1:
    return 'lib64'
  else: 
    return 'lib'
    
def get_arch():
  machine=os.uname()[4]
  if machine.find('x86_64') !=-1:
    return '64bit'
  else: 
    return '32bit'

def clean(standalonedir):
  if os.path.exists(standalonedir):
    shutil.rmtree(standalonedir)

def copy_stage(stagedir,standalonedir):
  libdir=get_libdir()
  copytree_ex(os.path.join(stagedir,'bin'),os.path.join(standalonedir,'bin'))
  copytree_ex(os.path.join(stagedir,libdir),os.path.join(standalonedir,libdir))
  if os.path.exists(os.path.join(stagedir,'share')):
    copytree_ex(os.path.join(stagedir,'share'),os.path.join(standalonedir,'share'))

def collect_deps(binaries,exclude_list):
  dependencies=set()
  for binary in binaries:
    collect_deps_recursive_(binary,dependencies,exclude_list)
  return dependencies
  
def collect_deps_recursive_(binary,collected,exclude_list):
  ldd_stdout=subprocess.Popen(['ldd',binary],stdout=subprocess.PIPE).stdout
  output=ldd_stdout.readlines()
  new_dependencies=set()
  for entry in output:
    if entry.find("statically linked")!=-1:
      continue
    sp=entry.split('(0x')
    if len(sp)>0: # only continue if line is dependency
     sp2=sp[0].split('=>',1)[-1].split()
     if sp2: #ensure libname exists (fix for virtual linux-gate)
       text=sp2[0]
       if not text.split('.')[0].split('/')[-1] in exclude_list and not text in collected:
         collect_deps_recursive_(text,collected,exclude_list)
         collected.add(text) 
  

def copy_deps(binaries,standalonedir,exclude_list):
  dependencies=set()
  walk_list=os.walk(standalonedir)
  binaries_list=[]
  for dir_entry in walk_list:
    for file_entry in dir_entry[2]:
      if file_entry.endswith('.so') or file_entry in binaries:
        filepath=os.path.join(dir_entry[0],file_entry)
        binaries_list.append(filepath)  
  dependencies=collect_deps(binaries_list,exclude_list)
  for dep in dependencies:
    shutil.copy2(dep,os.path.join(standalonedir,get_libdir()))

def copy_examples(stagedir):
  #todo
  pass
def copy_qt_plugins():
  #todo
  pass

def process_scripts(scripts,standalonedir):
  for script in scripts:
    fp=open(os.path.join(standalonedir,'bin',script),'r')
    text=fp.read()
    fp.close()
    text=re.sub('#export PYTHONPATH','export PYTHONPATH',text)
    fp=open(os.path.join(standalonedir,'bin',script),'w')
    fp.write(text)
    fp.close()

def create_package(name,standalonedir):
  try:
    import pysvn
    try:
      client=pysvn.Client()
      svn_info=client.info('.')
      revstring='-rev'+str(svn_info['revision'.number])
    except pysvn.ClientError:
      print 'No svn directory found. Addin no revision number.'
      revstring=''
  except ImportError:
    print 'Pysvn not installed. Adding no revision number.'
    revstring=''
  tar_name=name+'-linux-'+get_arch()+revstring+'.tgz'
  if os.path.exists(tar_name):
    os.remove(tar_name)
  tar = tarfile.open(tar_name, "w:gz")
  tar.add(standalonedir,name+'-linux-'+get_arch()+revstring)
  tar.close()

def ignorer(dirpath,files):
  ignores=[]
  for f in files:
    base=os.path.splitext(f)[0]
    if f.endswith('.py'):
      if base+'.pyc' in files or base+'.pyo' in files:
        ignores.append(f)
    elif f.endswith('.pyc'):
      if base+'.pyo' in files:
        ignores.append(f)
  return ignores

def copy_python_dirs(standalonedir):
  dirs=sys.path[1:]
  dirs.sort() #get toplevel dir to the beginning
  toplevel=dirs[0]
  shutil.copytree(toplevel,os.path.join(standalonedir,get_libdir(),'python'),ignore=ignorer)
  for d in dirs[1:]:
    if not d.startswith(toplevel) and os.path.isdir(d):
      toplevel=d
      copytree_ex(toplevel,os.path.join(standalonedir,get_libdir(),'python'),ignore=ignorer)


def copytree_ex(source,destination, symlinks=False,ignore=None):
  if not os.path.exists(destination):
   shutil.copytree(source,destination,symlinks,ignore)
  elif os.path.isdir(destination):
    files=os.listdir(source)
    if ignore:
      ignores=ignore(source,files)
    else:
      ignores=[]
    for f in files:
      srcname=os.path.join(source,f)
      dstname=os.path.join(destination,f)
      if os.path.isdir(os.path.join(source,f)):
        copytree_ex(srcname,dstname,symlinks,ignore=ignorer)
      elif f not in ignores:
        if symlinks and os.path.islink(srcname):
          linkto = os.readlink(srcname)
          os.symlink(linkto, dstname)
        else:
          shutil.copy2(srcname, dstname)
  else:
    raise IOError("destination is not a directory")
    

       
############   main script ####################

stagedir='../../stage'
ost_stagedir='../../../openstructure/stage'
standalonedir=os.path.join(os.getcwd(),'standalone')
binaries=['gosty']
exclude_list=['ld-linux','libexpat','libgcc_s','libglib','cmov','libice','libSM','libX','libg','libGL','libfontconfig','libfreetype','libdrm','libxcb','libICE']
scripts=['dng','ost']

#compile_stage()

clean(standalonedir)
print 'copying stage'
copy_stage(stagedir,standalonedir)
print 'copying ost stage'
copy_stage(ost_stagedir,standalonedir)
print 'copying python'
shutil.copy(sys.executable,os.path.join(stagedir,'bin'))
copy_python_dirs(standalonedir)
print 'copying dependencies'
binaries.append(sys.executable)
copy_deps(binaries,standalonedir,exclude_list)
print 'copying examples'
copy_examples(stagedir)
print 'copying plugins'
copy_qt_plugins()
print 'processing scripts'
process_scripts(scripts,standalonedir)
print 'creating package'
create_package('openstructure','standalone')
