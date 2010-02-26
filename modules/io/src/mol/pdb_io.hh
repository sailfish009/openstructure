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
#ifndef OST_IO_PDB_IO_HH
#define OST_IO_PDB_IO_HH

namespace ost { namespace io {
  
/// \brief flags that incluence the behaviour of the PDBReader and PDBWriter
struct PDB {
  typedef enum {
    /// \brief skip faulty records
    /// 
    /// This flag tells the PDB loader to ignore faulty records. By default,
    /// faulty records abort import.
    SKIP_FAULTY_RECORDS=1,
    /// \brief do not import HETATM records
    NO_HETATMS=2,
    /// \brief enable writing of multiple models
    WRITE_MULTIPLE_MODELS=4,
    /// \brief enable for PQR
    PQR_FORMAT=8,
  } Type; 
};

typedef unsigned int PDBFlags;

}}

#endif
