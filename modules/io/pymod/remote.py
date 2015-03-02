#------------------------------------------------------------------------------
# This file is part of the OpenStructure project <www.openstructure.org>
#
# Copyright (C) 2008-2015 by the OpenStructure authors
#
# This library is free software; you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License as published by the Free
# Software Foundation; either version 3.0 of the License, or (at your option)
# any later version.
# This library is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this library; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
#------------------------------------------------------------------------------

import urllib2
import tempfile

from ost.io import LoadPDB, LoadMMCIF

class RemoteRepository:
  """
  A remote repository represents a structural database accessible through the 
  internet, e.g. the PDB or SWISS-MODEL template library.
  """
  def __init__(self, name, url_pattern, type, id_transform='upper'):
    self.name = name
    self.url_pattern = url_pattern
    self.type = type
    if type not in ('cif', 'pdb'):
      raise ValueError('only cif and pdb types are supported')
    self.id_transform = id_transform

  def URLForID(self, id):
    if self.id_transform == 'upper':
      id = id.upper()
    if self.id_transform == 'lower':
      id = id.lower()
    return self.url_pattern.replace('$ID', id)

  def Get(self, id):
    remote_url = self.URLForID(id)
    tmp_file_suffix = '.%s' % self.type
    if remote_url.endswith('.gz'):
      tmp_file_suffix+='.gz'

    try:
      connection = urllib2.urlopen(remote_url)
      if hasattr(connection, 'code'):
        status = connection.code
      else:
        status = connection.getcode()
    except urllib2.HTTPError, e:
      status = e.code
    msg = 'Could not load %s from %s (status code %d)'
    if status!=200:
      raise IOError(msg % (id , self.name ,status))
    tmp_file = tempfile.NamedTemporaryFile(suffix=tmp_file_suffix)
    contents = ''.join(connection)
    tmp_file.write(contents)
    tmp_file.flush()
    return tmp_file

  def Load(self, id):
    tmp_file = self.Get(id)
    if self.type == 'pdb':
      return LoadPDB(tmp_file.name)
    if self.type == 'cif':
      return LoadMMCIF(tmp_file.name)

REMOTE_REPOSITORIES = {
    'pdb' : RemoteRepository('pdb.org (PDB)', 'http://www.pdb.org/pdb/files/$ID.ent.gz',
                   type='pdb', id_transform='upper'),
    'smtl' : RemoteRepository('SMTL', 'http://beta.swissmodel.expasy.org/templates/$ID.pdb',
                   type='pdb', id_transform='lower'),
    'cif' : RemoteRepository('pdb.org (mmCIF)', 'http://www.pdb.org/pdb/files/$ID.cif.gz',
                   type='cif', id_transform='lower'),
    'pdb_redo' : RemoteRepository('pdbredo', 'http://www.cmbi.ru.nl/pdb_redo/$ID/$ID_besttls.pdb',
                   type='pdb'),
}

def RemoteGet(id, from_repo='pdb'):
  remote_repo = REMOTE_REPOSITORIES.get(from_repo, None) 
  if not remote_repo:
    raise ValueError('%s is not a valid repository' % from_repo)
  return remote_repo.Get(id)

def RemoteLoad(id ,from_repo='pdb'):
  remote_repo = REMOTE_REPOSITORIES.get(from_repo, None) 
  if not remote_repo:
    raise ValueError('%s is not a valid repository' % from_repo)
  return remote_repo.Load(id)
