import subprocess
import os

currdir=os.getcwd()
if currdir.find('deployment')==-1 or currdir.find('linux')==-1:
  print '\n'
  print 'ERROR: Please run this script from the deployment/linux directory'
  print '\n'
  sys.exit()

subprocess.call('rm -fr scripts/dng.in.pre* scripts/ost_cl.in.pre*',shell=True,cwd='../../')
subprocess.call('mv scripts/ost.in.backup scripts/ost.in',shell=True,cwd='../../')
subprocess.call('mv scripts/dng.in.backup scripts/dng.in',shell=True,cwd='../../')

