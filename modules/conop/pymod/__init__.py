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
from _ost_conop import *

# The 20 standard amino acids in no particular order
STANDARD_AMINOACIDS=(
 'ALA', 'ARG', 'ASN',
 'ASP', 'GLN', 'GLU',
 'LYS', 'SER', 'CYS',
 'MET', 'TRP', 'TYR',
 'THR', 'VAL', 'ILE',
 'LEU', 'GLY', 'PRO',
 'HIS', 'PHE',
)

def ConnectAll(ent):
  '''
  Uses the current default builder to connect the atoms of the entity, assign 
  torsions, and fill in missing or correct erroneous information such as the 
  chemical class of the residues and the atom's element.
  
  :param ent: A valid entity
  :type ent: :class:`~ost.mol.EntityHandle`
  '''
  conop_inst=Conopology.Instance()
  conop_inst.ConnectAll(conop_inst.GetBuilder("DEFAULT"), ent, 0)
  
def GetBuilder(name='DEFAULT'):
  '''
  Get registered builder by name
  
  :param name: The name of the builder
  
  :returns: The builder or None, if the builder doesn't exist
  '''
  return Conopology.Instance().GetBuilder(name)  

def RegisterBuilder(builder, name):
  '''
  Register builder to OpenStructure
  
  :param builder: A instance of :class:`Builder`
  
  :param name: The name of the builder
  '''
  conop_inst=Conopology.Instance()
  conop_inst.RegisterBuilder(builder, name)
  
def SetDefaultBuilder(builder_name):
  '''
  Set the builder with the given name as the default. You will have to register
  a builder with :func:`RegisterBuilder` before you will be able to set it as
  the default.

  :raises: :exc:`RuntimeError` when trying to set a builder as the default that
    has not been registered yet.
  '''
  conop_inst=Conopology.Instance()
  conop_inst.SetDefaultBuilder(builder_name)
