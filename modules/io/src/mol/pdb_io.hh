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
    /// \brief Join atom records into one residue, even if the atom records
    ///     are not sequential.
    /// 
    /// This is useful in the following case:
    /// 
    /// \verbatim
    /// ATOM     43  N  AALA P   4
    /// ATOM     45  CA AALA P   4 
    /// ATOM     47  C  AALA P   4
    /// ATOM     48  O  AALA P   4
    /// ATOM     49  N  APRO P   5
    /// ATOM     50  CD APRO P   5
    /// ATOM     53  CA APRO P   5
    /// ATOM     55  CB APRO P   5
    /// ATOM     58  CG APRO P   5
    /// ATOM     61  C  APRO P   5
    /// ATOM     62  O  APRO P   5
    /// ATOM    550  CB AALA P   4
    /// \endverbatim
    /// 
    /// By default, the atom 550 will start a new residue instead of being
    /// joined with atoms 43-48 into one residue.
    JOIN_SPREAD_ATOM_RECORDS=16,
    //// \brief keep track of the order of atom records
    /// 
    /// This option is mostly useful in combination with 
    /// PDB::JOIN_SPREAD_ATOM_RECORDS and CoordGroups.
    /// 
    /// The atoms are accessible via PDBReader::GetSequentialAtoms()
    SEQUENTIAL_ATOM_IMPORT=32
  } Type; 
};

typedef unsigned int PDBFlags;

}}

#endif
