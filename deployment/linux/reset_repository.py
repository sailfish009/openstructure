import subprocess
import os

currdir=os.getcwd()
if currdir.find('deployment')==-1 or currdir.find('linux')==-1:
  print '\n'
  print 'ERROR: Please run this script from the deployment/linux directory'
  print '\n'
  sys.exit()

subprocess.call('rm -fr scripts/ost_config.in.pre*',shell=True,cwd='../../')
subprocess.call('mv scripts/ost_config.in.backup scripts/ost_config.in',shell=True,cwd='../../')

