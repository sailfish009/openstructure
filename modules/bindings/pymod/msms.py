'''
MSMS module

Author: Tobias Schmidt

This module is for calculating MSMS surfaces as well as surface areas
(SESA, SASA) from OpenStructure using the external program MSMS.

How To Use This Module:
 1. Import it (e.g. as "from ost.bindings import msms")
 2. Use it (e.g. as "surface = MSMS.CalculateSurface(entity)"
                    "(sesa,sasa)= MSMS.CalculateSurfaceArea(entity)")

Requirement:
 - MSMS installed
'''

import tempfile
import subprocess
import os

from ost import io
from ost import mol
from ost import settings
from ost import geom

## \brief Method to check if MSMS executable is present
#
# \param msms_exe Explicit path to msms executable
# \param msms_env Environment variable pointing to msms executable
# \return         Path to the executable
# \exception      FileNotFound if executable is not found
def _GetExecutable(msms_exe, msms_env):
  return settings.Locate('msms', explicit_file_name=msms_exe,
                         env_name=msms_env)

## \brief Setup files for MSMS calculation in temporary directory
#
# \param entity      EntityHandle or EntityView to calculate surface
# \param selection   Calculate surface for subset of entity 
# \return            Touple containing temporary directory and msms input file
# \exception         RuntimeError if selection is not valid
def _SetupFiles(entity, selection):
  # create temporary directory
  tmp_dir_name=tempfile.mkdtemp()

  # select only heavy atoms if no_hydrogens is true
  entity_view=entity.Select(selection)
  if not entity_view.IsValid():
    raise RuntimeError, "Could not create view for selection (%s)"%(selection)

  # write entity to tmp file
  tmp_file_name=os.path.join(tmp_dir_name,"entity")
  tmp_file_handle=open(tmp_file_name, 'w')
  for a in entity_view.GetAtomList():
    position=a.GetPos()
    tmp_file_handle.write('%8.3f %8.3f %8.3f %4.2f\n' % (position[0],
                          position[1], position[2], a.GetProp().radius))
  tmp_file_handle.close()
  
  return (tmp_dir_name, tmp_file_name)

## \brief Method which recursively deletes a directory
#
# \warning This method removes also non-empty directories without asking, so
#          be careful!
def __CleanupFiles(dir_name):
  import shutil
  shutil.rmtree(dir_name)

## \brief Method to run the MSMS surface calculation
#
# This method starts the external MSMS executable and returns the stdout of MSMS
#
# \param command          Command to execute
# \return                 stdout of MSMS
# \exception              CalledProcessError for non-zero return value
def _RunMSMS(command):
  proc = subprocess.Popen(command, shell=True, stdout=subprocess.PIPE)
  stdout_value, stderr_value = proc.communicate()

  #check for successful completion of msms
  if proc.returncode!=0:
    print "WARNING: msms error\n", stdout_value
    raise subprocess.CalledProcessError(proc.returncode, command)

  return stdout_value
  

## \brief Calculates analytical solvent excluded and solvent accessible surface
#  area by using the external MSMS program
#
# This method calculates the molecular surface areas by invoking the external
# program MSMS. First, it is checked if the MSMS executable is present, then, 
# the necessary files are prepared in a temporary directory and MSMS is
# executed. The last step is to remove the temporary directory.
# 
#
# \param entity        OST entity to calculate surface
# \param density       Surface point density
# \param radius        Surface probe radius
# \param all_surf      Calculate surface area for all cavities (returns multiple
#                      surfaces areas as a list)
# \param no_hydrogens  Calculate surface only for hevy atoms
# \param selection     Calculate surface for subset of entity
# \param msms_exe      msms executable (full path to executable)
# \param msms_env      msms environment variable
# \param keep_files    Do not delete temporary files
# \return              Touplet of lists for (SES, SAS)
def CalculateSurfaceArea(entity, density=1.0, radius=1.5,  all_surf=False,
                         no_hydrogens=False, selection="",
                         msms_exe=None, msms_env=None, keep_files=False):
  import re 

  # check if msms executable is specified
  msms_executable=_GetExecutable(msms_exe, msms_env)

  # parse selection
  if no_hydrogens:
    selection+=" and ele!=H"

  # setup files for msms
  (msms_data_dir, msms_data_file)=_SetupFiles(entity, selection)

  # set command line
  command="%s -if %s -of %s -density %s -probe_radius %s -surface ases" % \
          (msms_executable, msms_data_file, msms_data_file, density, radius)
  if all_surf:
    command+=" -all"

  # run msms
  stdout_value=_RunMSMS(command)

  # parse MSMS output
  msms_ases=[]
  msms_asas=[]
  data_paragraph=False
  for line in stdout_value.splitlines():
    if re.match('MSMS terminated normally', line):
      data_paragraph=False
    if data_paragraph:
      (ses_,sas_)=line.split()[5:7]
      msms_ases.append(float(ses_))
      msms_asas.append(float(sas_))
    if re.match('    Comp. probe_radius,   reent,    toric,   contact    SES       SAS', line):
      data_paragraph=True

  # clean up
  if not keep_files:
    __CleanupFiles(msms_data_dir)

  return (msms_ases, msms_asas)
  

## \brief Calculates molecular surface by using the external MSMS program
#
# This method calculates a molecular surface by invoking the external program
# MSMS. First, it is checked if the MSMS executable is present, then, the
# necessary files are prepared in a temporary directory and MSMS is executed.
# The last step is to remove the temporary directory.
# 
#
# \param entity        OST entity to calculate surface
# \param density       Surface point density
# \param radius        Surface probe radius
# \param all_surf      Calculate surface for all cavities (returns multiple
#                      surfaces as a list)
# \param no_hydrogens  Calculate surface only for hevy atoms
# \param selection     Calculate surface for subset of entity
# \param msms_exe      msms executable (full path to executable)
# \param msms_env      msms environment variable
# \param keep_files    Do not delete temporary files
# \return list of OST SurfaceHandle objects
def CalculateSurface(entity, density=1.0, radius=1.5, all_surf=False,
                     no_hydrogens=False, selection="",
                     msms_exe=None, msms_env=None, keep_files=False):
  import os
  import re

  # check if msms executable is specified
  msms_executable=_GetExecutable(msms_exe, msms_env)

  # parse selection
  if no_hydrogens:
    selection+=" and ele!=H"

  # setup files for msms
  print selection
  (msms_data_dir, msms_data_file)=_SetupFiles(entity, selection)

  # set command line
  command="%s -if %s -of %s -density %s -probe_radius %s" % (msms_executable,
          msms_data_file, msms_data_file, density, radius)
  if all_surf:
    command+=" -all"

  # run msms
  stdout_value=_RunMSMS(command)

  # parse msms output
  num_surf=0
  for line in stdout_value.splitlines():
    if re.search('RS component [0-9]+ identified',line):
      num_surf=int(line.split()[2])

  # get surfaces
  msms_surfaces=[]
  msms_surfaces.append(io.LoadSurface(msms_data_file, "msms"))
  for n in range(1,num_surf+1):
    filename=msms_data_file+'_'+str(n)
    msms_surfaces.append(io.LoadSurface(filename, "msms"))

  # clean up
  if not keep_files:
    __CleanupFiles(msms_data_dir)

  return msms_surfaces

