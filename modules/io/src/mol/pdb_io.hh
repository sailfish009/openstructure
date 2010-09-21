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

#include <stack>
#include <ost/io/module_config.hh>

namespace ost { namespace io {
  

/// \brief flags that incluence the behaviour of the PDBReader and PDBWriter
struct DLLEXPORT_OST_IO PDB {

    /// \brief skip faulty records
    /// This flag tells the PDB loader to ignore faulty records. By default,
    /// faulty records abort import.
  static const unsigned int SKIP_FAULTY_RECORDS;

    /// \brief do not import HETATM records
  static const unsigned int NO_HETATMS;

    /// \brief enable writing of multiple models
  static const unsigned int WRITE_MULTIPLE_MODELS;

    /// \brief enable for PQR
  static const unsigned int PQR_FORMAT;

  /// \brief enables parsing of charmm-style PDBs
  /// 
  /// CHARMM files don't use the chain column to mark different chains, but 
  /// rather put the name in the last columns that is isually used for the atom 
  /// element an occupancy. Note that it is perfectly possible to parse CHARRM 
  /// PDB files without this flag, but the mileage may vary as some of the 
  /// elements are incorrectly assigned.
  static const unsigned int CHARMM_FORMAT;
  
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
  static const unsigned int JOIN_SPREAD_ATOM_RECORDS;

    /// \brief only import C-alpha atoms
  static const unsigned int CALPHA_ONLY;

  static void PushFlags(unsigned int flags);
  static unsigned int Flags();
  static void PopFlags();

  static PDB& Instance();

  std::stack<unsigned int> fstack;
};

}}

#endif
