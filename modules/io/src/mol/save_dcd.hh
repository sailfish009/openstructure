//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2010 by the OpenStructure authors
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License as published by the Free
// Software Foundation; either version 3.0 of the License, or (at your option)
// any later version.
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
// details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
//------------------------------------------------------------------------------
#ifndef OST_SAVE_DCD_HH
#define OST_SAVE_DCD_HH

/*
  Author: Marco Biasini
 */
 
#include <ost/io/module_config.hh>
#include <ost/mol/coord_group.hh>

namespace ost { namespace io {
  
/// \brief export coord group as PDB file and DCD trajectory
void DLLEXPORT_OST_IO SaveCHARMMTraj(const mol::CoordGroupHandle& coord_group, 
                                     const String& pdb_filename, 
                                     const String& dcd_filename,
                                     unsigned int stepsize=1);
}}

#endif
