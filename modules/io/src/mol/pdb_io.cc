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
#include "pdb_io.hh"

namespace ost { namespace io {

  PDB& PDB::Instance() {
    static PDB x;
    return x;
  }

  const unsigned int PDB::SKIP_FAULTY_RECORDS=0x1;
  const unsigned int PDB::NO_HETATMS=0x2;
  const unsigned int PDB::WRITE_MULTIPLE_MODELS=0x4;
  const unsigned int PDB::PQR_FORMAT=0x8;
  const unsigned int PDB::JOIN_SPREAD_ATOM_RECORDS=0x10;
  const unsigned int PDB::CALPHA_ONLY=0x20;
  const unsigned int PDB::CHARMM_FORMAT=0x40;  

  void PDB::PushFlags(unsigned int flags) {PDB::Instance().fstack.push(flags);}
  unsigned int PDB::Flags() {return PDB::Instance().fstack.empty() ? 0 : PDB::Instance().fstack.top();}
  void PDB::PopFlags() {PDB::Instance().fstack.pop();}


}} // ns
