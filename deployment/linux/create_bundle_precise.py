import os
import shutil
import subprocess
import string
import sys
import datetime
import sip
import PyQt4


# parameters that can be modified
qt4_plugins_location='/usr/lib/i386-linux-gnu/qt4/plugins'
ssl_crypto_location='/lib/i386-linux-gnu/'
chemlib_dictionary_location='/home/bundler/compounds.chemlib'
list_of_excluded_libraries=[
'ld-linux',
'libexpat',
'libgcc_s',
'libglib',
'cmov',
'libice',
'libSM',
'libX',
'libg',
'libGL.so',
'libfontconfig',
'libfreetype',
'libdrm',
'libxcb',
'libICE',
'libnvidia',
'libc'
]

if len(sys.argv) < 2:
  print 'usage: create_bundle.py  additional_label'
  sys.exit()

additional_label=sys.argv[1]

currdir=os.getcwd()
if currdir.find('deployment')==-1 or currdir.find('linux')==-1:
  print '\n'
  print 'ERROR: Please run this script from the deployment/linux directory'
  print '\n'
  sys.exit()

print "WARNING: If this script does not run to completion, please run 'git reset --hard HEAD' in the main directory (two up from here) before restarting"

system_python_version=sys.version_info
system_python_executable='python'+str(system_python_version[0])+'.'+str(system_python_version[1])
system_python_bin='/usr/bin/'+system_python_executable
system_python_libs='/usr/lib/'+system_python_executable
qt4_module_location=os.path.dirname(PyQt4.__file__)
sip_module_location=os.path.dirname(sip.__file__)
print '\n'
print '\n'
print 'Detecting architecture and revision'
uname_output=subprocess.Popen('uname -a', shell=True, cwd='../../',stdout=subprocess.PIPE).stdout
uname_line=uname_output.readlines()
if uname_line[0].find('x86_64') !=-1:
  libdir='lib64'
  archstring='64bit'
else: 
  libdir='lib'
  archstring='32bit'
directory_name='openstructure-linux-'+archstring+'-'+additional_label
print 'Hardcoding package python binary path in openstructure executables'
subprocess.call('rm -f scripts/ost_config.in.pre* scripts/ost_config.in.backup',shell=True,cwd='../../')
subprocess.call('mv scripts/ost_config.in scripts/ost_config.in.backup',shell=True,cwd='../../')
subprocess.call('sed "s/@PYTHON_BINARY@/\$DNG_ROOT\/bin\/python/g" scripts/ost_config.in.backup > scripts/ost_config.in.preprepre',shell=True,cwd='../../')
subprocess.call('sed "s/\#export PYTHONHOME/ export PYTHONHOME/g" scripts/ost_config.in.preprepre > scripts/ost_config.in.prepre',shell=True,cwd='../../')
subprocess.call('sed "s/\#export PYTHONPATH/ export PYTHONPATH/g" scripts/ost_config.in.prepre > scripts/ost_config.in.pre',shell=True,cwd='../../')
subprocess.call('sed "s/\#export QT_PLUGIN_PATH/ export QT_PLUGIN_PATH/g" scripts/ost_config.in.pre > scripts/ost_config.in',shell=True,cwd='../../')
print 'Compiling Openstructure'
subprocess.call('cmake ./ -DCMAKE_BUILD_TYPE=Release -DPREFIX='+directory_name+' -DCOMPOUND_LIB='+chemlib_dictionary_location+' -DUSE_RPATH=ON -DENABLE_IMG=ON -DENABLE_GUI=ON -DENABLE_GFX=ON -DOPTIMIZE=ON',shell=True,cwd='../../')
subprocess.call('make -j2',shell=True,cwd='../../')
print 'Removing obsolete packages and package directory'
subprocess.call('rm -fr openstructure-linux*',shell=True,cwd='../../')
print 'Creating new package directory'
subprocess.call('mkdir '+directory_name,shell=True,cwd='../../')
subprocess.call('mkdir '+directory_name+'/bin',shell=True,cwd='../../')
print 'Copy python executable into stage for dependency detection'
subprocess.call('cp '+system_python_bin+ ' stage/bin/python',shell=True,cwd='../../')
print 'Copy python libraries into the stage for dependency detection'
subprocess.call('cp -pRL '+system_python_libs+' stage/'+libdir+'/',shell=True,cwd='../../')
subprocess.call('rm -fr stage/'+libdir+'/'+system_python_executable+'/dist-packages',shell=True,cwd='../../')
subprocess.call('cp -pRL '+sip_module_location+'/sip* stage/'+libdir+'/'+system_python_executable+'/',shell=True,cwd='../../')
subprocess.call('cp -pRL '+qt4_module_location+' stage/'+libdir+'/'+system_python_executable+'/',shell=True,cwd='../../')
print 'Copy boost program option libraries into the stage for dependency detection'
subprocess.call('cp -pRL /usr/lib/libboost_program_options.so* stage/'+libdir+'/',shell=True,cwd='../../')
print 'Creating new dependency list'
so_list=[]
walk_list=os.walk('../../stage')
for dir_entry in walk_list:
  for file_entry in dir_entry[2]:
    if file_entry.find('.so')==len(file_entry)-3 or file_entry=='gosty':
      filepath=os.path.join(dir_entry[0],file_entry)
      so_list.append(filepath)  
dep_list=[]
for so_entry in so_list:
   dep=subprocess.Popen('perl ./ldd-rec.pl '+so_entry,shell=True,stdout=subprocess.PIPE,cwd='./').stdout
   dep_so_entry=dep.readlines()
   for dep_entry in dep_so_entry:
     dep_list.append(dep_entry[:-1]) 
sorted_dep_list=sorted(list(set(dep_list)))
print 'Filtering system libraries from depenedency list'
filtered_dep_list=[]
for entry in sorted_dep_list:
   exclude=False
   for exclusion in list_of_excluded_libraries:
     if entry.find(exclusion)!=-1:
       exclude=True
   if exclude==False: 
     filtered_dep_list.append(entry)
print 'Installing OpenStructure into package directory structure'
subprocess.call('make install',shell=True,cwd='../../')
print 'Copy libraries in the package directory structure'
for entry in filtered_dep_list:
  subprocess.call('cp '+entry+' '+directory_name+'/'+libdir,shell=True,cwd='../../')
print 'Copy python executable into package directory structure'
subprocess.call('cp '+system_python_bin+ ' '+directory_name+'/bin/python',shell=True,cwd='../../')
print 'Copy python libraries into package directory structure'
subprocess.call('cp -pRL '+system_python_libs+' '+directory_name+'/'+libdir+'/'+system_python_executable+'/',shell=True,cwd='../../')
subprocess.call('rm -fr '+directory_name+'/'+libdir+'/'+system_python_executable+'/dist-packages',shell=True,cwd='../../')
subprocess.call('cp -pRL '+sip_module_location+'/sip* '+directory_name+'/'+libdir+'/'+system_python_executable+'/',shell=True,cwd='../../')
subprocess.call('cp -pRL '+qt4_module_location+' '+directory_name+'/'+libdir+'/'+system_python_executable+'/',shell=True,cwd='../../')
print 'Copy Qt 4 plugins into package directory structure'
subprocess.call('cp -pRL '+qt4_plugins_location+' '+directory_name+'/bin/',shell=True,cwd='../../')
print 'Copying supplementary material into package directory structure'
subprocess.call('cp -pRL  stage/share/openstructure  '+directory_name+'/share/',shell=True,cwd='../../')
print 'Copying examples into package directory structure'
subprocess.call('cp -pRL  examples  '+directory_name+'/share/openstructure/',shell=True,cwd='../../')
print 'Copying ReadMe file into package directory structure'
subprocess.call('cp deployment/README.html '+directory_name,shell=True,cwd='../../')
print 'Creating executables at the top level of the package directory structure'
subprocess.call('ln -sf bin/dng ./dng',shell=True,cwd='../../'+directory_name)
subprocess.call('ln -sf bin/ost ./ost',shell=True,cwd='../../'+directory_name)
subprocess.call('ln -sf bin/lddt ./lddt',shell=True,cwd='../../'+directory_name)
print 'Copying additional libraries in the package directory structure'
subprocess.call('cp '+ssl_crypto_location+'/libssl.so* '+directory_name+'/'+libdir,shell=True,cwd='../../')
subprocess.call('cp '+ssl_crypto_location+'/libcrypto.so* '+directory_name+'/'+libdir,shell=True,cwd='../../')
subprocess.call('cp -pRL /usr/lib/libboost_program_options.so* '+directory_name+'/'+libdir+'/',shell=True,cwd='../../')
print 'Copying python headers in the package directory structure'
subprocess.call('mkdir -p '+directory_name+'/local/include/',shell=True,cwd='../../')
subprocess.call('cp -r /usr/include/'+system_python_executable+' '+directory_name+'/local/include/',shell=True,cwd='../../')
print 'Stripping pyc files from bundle'
subprocess.call('rm -rf $(find . -name *.pyc)',shell=True,cwd='../../')
print 'removing dokk and harmony examples from bundle'
subprocess.call('rm -rf '+directory_name+'/share/openstructure/examples/code_fragments/dokk',shell=True,cwd='../../')
subprocess.call('rm -rf '+directory_name+'/share/openstructure/examples/code_fragments/harmony',shell=True,cwd='../../')
print 'Compressing the bundle'
subprocess.call('tar cfz '+directory_name+'.tgz '+directory_name,shell=True,cwd='../../')
print "Done. Please run 'git reset --hard HEAD' in the top directory before restarting."

