import os
import shutil
import subprocess
import string
import sys
import datetime


# custom parameters

if len(sys.argv) < 2:
  print 'usage: create_bundle.py  additional_label'
  sys.exit()

system_python_version='python2.6'
system_python_bin='/usr/bin/'+system_python_version
system_python_libs='/usr/lib/'+system_python_version
second_system_python_libs_flag=True
second_system_python_libs='/usr/lib/pymodules/'+system_python_version
python_bin_in_bundle='python'
qt4_plugins='/usr/lib/qt4/plugins'
additional_label=sys.argv[1]
list_of_excluded_libraries=['ld-linux','libexpat','libgcc_s','libglib','cmov','libice','libSM','libX','libg','libGL.so','libfontconfig','libfreetype','libdrm','libxcb','libICE','libnvidia','libc']
currdir=os.getcwd()
if currdir.find('deployment')==-1 or currdir.find('linux')==-1:
  print '\n'
  print 'ERROR: Please run this script from the deployment/linux directory'
  print '\n'
  sys.exit()
print '\n'
print 'WARNING: If this script does not run to completion, please run the reset_repository.py script before restarting'
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
date_pattern='%Y-%b-%d'
build=datetime.date.today()
directory_name='openstructure-linux-'+archstring+'-'+additional_label+'-'+str(build)
print 'Hardcoding package python binary path in openstructure executables'
subprocess.call('mv scripts/ost.in scripts/ost.in.backup',shell=True,cwd='../../')
subprocess.call('sed "s/@PYTHON_BINARY@/\$DNG_ROOT\/bin\/'+python_bin_in_bundle+'/g" scripts/ost.in.backup > scripts/ost.in.prepreprepre',shell=True,cwd='../../')
subprocess.call('sed "s/\#export LD_LIBRARY_PATH/ export LD_LIBRARY_PATH/g" scripts/ost.in.prepreprepre > scripts/ost.in.preprepre',shell=True,cwd='../../')
subprocess.call('sed "s/\#export PYTHONHOME/ export PYTHONHOME/g" scripts/ost.in.preprepre > scripts/ost.in.prepre',shell=True,cwd='../../')
subprocess.call('sed "s/\#export PYTHONPATH/ export PYTHONPATH/g" scripts/ost.in.prepre > scripts/ost.in.pre',shell=True,cwd='../../')
subprocess.call('sed "s/\#export QT_PLUGIN_PATH/ export QT_PLUGIN_PATH/g" scripts/ost.in.pre > scripts/ost.in',shell=True,cwd='../../')
subprocess.call('mv scripts/dng.in scripts/dng.in.backup',shell=True,cwd='../../')
subprocess.call('sed "s/\#export LD_LIBRARY_PATH/ export LD_LIBRARY_PATH/g" scripts/dng.in.backup > scripts/dng.in.preprepre',shell=True,cwd='../../')
subprocess.call('sed "s/\#export PYTHONHOME/ export PYTHONHOME/g" scripts/dng.in.preprepre > scripts/dng.in.prepre',shell=True,cwd='../../')
subprocess.call('sed "s/\#export PYTHONPATH/ export PYTHONPATH/g" scripts/dng.in.prepre > scripts/dng.in.pre',shell=True,cwd='../../')
subprocess.call('sed "s/\#export QT_PLUGIN_PATH/ export QT_PLUGIN_PATH/g" scripts/dng.in.pre > scripts/dng.in',shell=True,cwd='../../')
#print 'Downloading Chemlib dictionary'
#subprocess.call('wget ftp://ftp.wwpdb.org/pub/pdb/data/monomers/components.cif', shell=True, cwd='../../')
#print 'Compiling Openstructure'
#subprocess.call('cmake ./ -DCMAKE_BUILD_TYPE=Release -DPREFIX='+directory_name+' -DBoost_COMPILER='+boost_string+'-DENABLE_IMG=OFF -DENABLE_UI=OFF -DENABLE_GFX=OFF', shell=True,cwd='../../')
#subprocess.call('cmake ./ -DCMAKE_BUILD_TYPE=Release -DPREFIX='+directory_name+' s-DENABLE_IMG=OFF -DENABLE_UI=OFF -DENABLE_GFX=OFF', shell=True,cwd='../../')
#subprocess.call('make -j5',shell=True,cwd='../../')
#print 'Converting Chemlib dictionary'
#subprocess.call('stage/bin/chemdict_tool create components.cif compounds.chemlib', shell=True, cwd='../../')
#print '\nStaging Chemlib dictionary'
print 'Compiling Openstructure'
#subprocess.call('cmake ./ -DCMAKE_BUILD_TYPE=Release -DPREFIX='+directory_name+' -DBoost_COMPILER='+boost_string+' -DCOMPOUND_LIB=ChemLib/compounds.chemlib -DENABLE_IMG=ON -DENABLE_UI=ON -DENABLE_GFX=ON -DOPTIMIZE=ON',shell=True,cwd='../../')
subprocess.call('cmake ./ -DCMAKE_BUILD_TYPE=Release -DPREFIX='+directory_name+' -DCOMPOUND_LIB=ChemLib/compounds.chemlib -DENABLE_IMG=ON -DENABLE_UI=ON -DENABLE_GFX=ON -DOPTIMIZE=ON',shell=True,cwd='../../')
subprocess.call('make -j2',shell=True,cwd='../../')
print 'Removing obsolete packages and package directory'
subprocess.call('rm -fr openstructure-linux*',shell=True,cwd='../../')
print 'Creating new package directory'
subprocess.call('mkdir '+directory_name,shell=True,cwd='../../')
subprocess.call('mkdir '+directory_name+'/bin',shell=True,cwd='../../')
print 'Copy python executable into stage for dependency detection'
subprocess.call('cp '+system_python_bin+ ' stage/bin/python',shell=True,cwd='../../')
print 'Copy python libraries into the stage for dependency detection'
subprocess.call('cp -pRL '+system_python_libs+' stage/'+libdir,shell=True,cwd='../../')
subprocess.call('rm -fr stage/'+libdir+'/'+system_python_version+'/dist-packages',shell=True,cwd='../../')
if second_system_python_libs_flag==True:
  subprocess.call('cp -pRL '+second_system_python_libs+'/sip* stage/'+libdir+'/'+system_python_version,shell=True,cwd='../../')
  subprocess.call('cp -pRL '+second_system_python_libs+'/PyQt* stage/'+libdir+'/'+system_python_version,shell=True,cwd='../../')
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
print 'Copy libraries in the package directory structure'
subprocess.call('mkdir '+directory_name+'/'+libdir,shell=True,cwd='../../')
for entry in filtered_dep_list:
  subprocess.call('cp '+entry+' '+directory_name+'/'+libdir,shell=True,cwd='../../')
print 'Copy python executable into package directory structure'
subprocess.call('cp '+system_python_bin+ ' '+directory_name+'/bin/python',shell=True,cwd='../../')
print 'Copy python libraries into package directory structure'
subprocess.call('cp -pRL '+system_python_libs+' '+directory_name+'/'+libdir,shell=True,cwd='../../')
subprocess.call('rm -fr '+directory_name+'/'+libdir+'/'+system_python_version+'/dist-packages',shell=True,cwd='../../')
if second_system_python_libs_flag==True:
  subprocess.call('cp -pRL '+second_system_python_libs+'/sip* '+directory_name+'/'+libdir+'/'+system_python_version,shell=True,cwd='../../')
  subprocess.call('cp -pRL '+second_system_python_libs+'/PyQt* '+directory_name+'/'+libdir+'/'+system_python_version,shell=True,cwd='../../')
print 'Copy Qt 4 plugins into package directory structure'
subprocess.call('cp -pRL '+qt4_plugins+' '+directory_name+'/bin/',shell=True,cwd='../../')
print 'Installing OpenStructure into package directory structure'
subprocess.call('make install',shell=True,cwd='../../')
print 'Copying supplementary material into package directory structure'
subprocess.call('cp -pRL  stage/share/openstructure  '+directory_name+'/share/',shell=True,cwd='../../')
print 'Copying examples into package directory structure'
subprocess.call('cp -pRL  examples  '+directory_name+'/share/openstructure/',shell=True,cwd='../../')
print 'Copying ReadMe file into package directory structure'
subprocess.call('cp deployment/README.html '+directory_name,shell=True,cwd='../../')
print 'Creating executables at the top level of the package directory structure'
subprocess.call('ln -sf bin/dng ./dng',shell=True,cwd='../../'+directory_name)
subprocess.call('ln -sf bin/ost ./ost',shell=True,cwd='../../'+directory_name)
print 'Copying additional libraries in the package directory structure'
subprocess.call('cp /usr/lib/libssl.so.0.9.8   '+directory_name+'/'+libdir,shell=True,cwd='../../')
subprocess.call('cp /usr/lib/libcrypto.so.0.9.8   '+directory_name+'/'+libdir,shell=True,cwd='../../')
print 'Removing headers from package directory structure'
subprocess.call('rm -fr   '+directory_name+'/include',shell=True,cwd='../../')
print 'Stripping pyc files from bundle'
subprocess.call('rm -rf $(find . -name *.pyc)',shell=True,cwd='../../')
print 'removing dokk and harmony examples from bundle'
subprocess.call('rm -rf '+directory_name+'/share/openstructure/examples/code_fragments/dokk',shell=True,cwd='../../')
subprocess.call('rm -rf '+directory_name+'/share/openstructure/examples/code_fragments/harmony',shell=True,cwd='../../')
print 'De-hardcoding package python binary path from openstructure executables'
subprocess.call('rm scripts/ost.in',shell=True,cwd='../../')
subprocess.call('rm scripts/ost.in.pre*',shell=True,cwd='../../')
subprocess.call('rm scripts/dng.in',shell=True,cwd='../../')
subprocess.call('rm scripts/dng.in.pre*',shell=True,cwd='../../')
subprocess.call('mv scripts/ost.in.backup scripts/ost.in',shell=True,cwd='../../')
subprocess.call('mv scripts/dng.in.backup scripts/dng.in',shell=True,cwd='../../')
subprocess.call('tar cfz '+directory_name+'.tgz '+directory_name,shell=True,cwd='../../')

