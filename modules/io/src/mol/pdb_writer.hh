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
#ifndef OST_IO_PDB_WRITER_HH
#define OST_IO_PDB_WRITER_HH


/*
  Author: Marco Biasini
 */
#include <fstream>

#include <boost/filesystem/fstream.hpp>
#include <boost/iostreams/filtering_stream.hpp>

#include <ost/io/module_config.hh>

#include <ost/mol/mol.hh>

#include "pdb_io.hh"

namespace ost { namespace io {

class DLLEXPORT_OST_IO PDBWriter {
public:
  PDBWriter(const String& filename);
  PDBWriter(const boost::filesystem::path& filename);
  PDBWriter(std::ostream& outstream);
  
  void Write(const mol::EntityView& ent);
  void Write(const mol::EntityHandle& ent);
  
  void Write(const mol::AtomHandleList& atoms);

  ~PDBWriter();
private:
  template <typename H>
  void WriteModel(H ent);
  
  void WriteModelLeader();
  void WriteModelTrailer();
  std::ofstream  outfile_;
  std::ostream&   outstream_;
  int mol_count_;
  std::map<long, int> atom_indices_;
};
 
}}

#endif
