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
#ifndef OST_IO_PDB_READER_HH
#define OST_IO_PDB_READER_HH
/*
  Author: Marco Biasini
 */

#include <boost/iostreams/filtering_stream.hpp>
#include <boost/filesystem/fstream.hpp>

#include <ost/mol/mol.hh>
#include <ost/mol/xcs_editor.hh>
#include <ost/io/module_config.hh>
#include <ost/io/mol/pdb_io.hh>

namespace ost { namespace io {

class DLLEXPORT_OST_IO PDBReader {
  struct HSEntry {
    mol::ResNum start;
    mol::ResNum end;
    String chain;
  };
  typedef std::vector<HSEntry> HSList;
public:
  PDBReader(const String& filename);
  PDBReader(const boost::filesystem::path& loc);
  PDBReader(std::istream& instream);

  bool HasNext();

  void Import(mol::EntityHandle& ent,
              const String& restrict_chains="");
  void SetFlags(PDBFlags flags);
  // these two are needed for synchronizing with a traj import
  void CollectSequentialAtoms(bool f);
  std::vector<mol::AtomHandle> GetSequentialAtoms() const;

private:
  void ClearState();

  void ParseAndAddAtom(const String& line, int line_num,
                       mol::EntityHandle& h, const String& record_type);

  void ParseHelixEntry(const String& line);
  void ParseStrandEntry(const String& line);
  void Init(const boost::filesystem::path& loc);
  mol::ChainHandle curr_chain_;
  mol::ResidueHandle curr_residue_;
  int chain_count_;
  int residue_count_;
  int atom_count_;
  int line_num_;
  String restrict_chains_;
  HSList helix_list_;
  HSList strand_list_;

  boost::filesystem::ifstream infile_;
  std::istream& instream_;
  boost::iostreams::filtering_stream<boost::iostreams::input>  in_;
  String curr_line_;
  bool collect_sequential_atoms_;
  std::vector<mol::AtomHandle> sequential_atom_list_;
  PDBFlags flags_;
  // this needs to be set to true for reading pqr
  // file (i.e. pdb formatted file with charges in occupacy
  // column, and radii in b-factor column)
  bool is_pqr_;
};

}}

#endif
