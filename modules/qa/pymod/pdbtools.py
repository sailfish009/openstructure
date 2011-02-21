#------------------------------------------------------------------------------
# This file is part of the OpenStructure project <www.openstructure.org>
#
# Copyright (C) 2008-2011 by the OpenStructure authors
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
import httplib
import os.path
import types
def GetModelsFromPISCES(pisces_list, skip_header=True):
  """
  generator function to iterate over models and chains listed in a PISCES
  file.
  """
  pisces=open(pisces_list)
  if skip_header:
    pisces.readline()
  for line in pisces:
    if line=='':
      continue
    pdb_id_and_chain=line.split(' ')[0]
    yield pdb_id_and_chain[:4], pdb_id_and_chain[4:]
  pisces.close()
    
def RetrieveModels(models, output_dir, file_pattern='pdb%s.ent'):
  """
  Retrieve one or more pdb models from the pdb.org server.
  
  models must be a pdb id or list of pdb ids
  """
  is_tuple=isinstance(models, types.TupleType)
  is_list=isinstance(models, types.ListType)
  is_generator=isinstance(models, types.GeneratorType)
  if not (is_list or is_tuple or is_generator):
    models=[models]
  conn=httplib.HTTPConnection('www.pdb.org')
  for model_id, chain in models:
    print 'getting file for model with id=%s...' % model_id.lower(),
    conn.request('GET', '/pdb/files/%s.pdb' % model_id )
    response=conn.getresponse()
    print response.reason
    if response.status==200:
      data=response.read()
      f=open(os.path.join(output_dir, file_pattern % model_id), 'w+')
      f.write(data)
      f.close()
  conn.close()    