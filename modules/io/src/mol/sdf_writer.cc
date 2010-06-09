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
/*
  Author: Tobias Schmidt
 */

#include "sdf_writer.hh"

namespace ost { namespace io {

using boost::format;

namespace {

  class SDFAtomWriter : public mol::EntityVisitor {
    public:
      SDFAtomWriter(std::ostream& ostream, std::map<long, int>& atom_indices)
      : ostr_(ostream), atom_indices_(atom_indices), counter_(0) {
  atom_indices_.clear();
      }
    private:
    public:
      virtual bool VisitAtom(const mol::AtomHandle& atom) {
        atom_indices_[atom.GetHashCode()] = ++counter_;
        ostr_ << format("%10.4f") % atom.GetPos()[0]
              << format("%10.4f") % atom.GetPos()[1]
              << format("%10.4f ") % atom.GetPos()[2]
              << format("%-3s") % atom.GetElement()
              << " 0  0  0  0  0  0"
              << std::endl;
        return true;
      }
    private:
      std::ostream&      ostr_;
      std::map<long, int>& atom_indices_;
      int counter_;
  };

  class SDFBondWriter : public mol::EntityVisitor {
  public:
    SDFBondWriter(std::ostream& ostream, std::map<long, int>& atom_indices)
      : ostr_(ostream), atom_indices_(atom_indices), counter_(0) {
    }
  private:
  public:
    virtual bool VisitAtom(const mol::AtomHandle& atom) {
      counter_++;
      mol::AtomHandleList atoms = atom.GetBondPartners();
      mol::AtomHandleList::iterator atom_iter = atoms.begin();
      for(; atom_iter != atoms.end(); ++atom_iter) {
        int atom_index = atom_indices_.find((*atom_iter).GetHashCode())->second;
        if(atom_index > counter_) {
          int type = 1;
          mol::BondHandle bond = atom.FindBondToAtom(*atom_iter);
          if(bond.IsValid()) type = bond.GetBondOrder();
          ostr_ << format("%3i") % counter_
                << format("%3i") % atom_index
                << format("%3i") % type
                << "  0  0  0"
                << std::endl;
        }
      }
      return true;
    }

  private:
    std::ostream&      ostr_;
    std::map<long, int>& atom_indices_;
    int counter_;
  };
}

SDFWriter::SDFWriter(std::ostream& ostream)
  : outfile_(), ostr_(ostream), counter_(0) {
}

SDFWriter::SDFWriter(const String& filename)
  : outfile_(filename.c_str()), ostr_(outfile_), counter_(0) {
}

SDFWriter::SDFWriter(const boost::filesystem::path& filename)
  : outfile_(filename.file_string().c_str()), ostr_(outfile_), counter_(0) {
}

void SDFWriter::Write(const mol::EntityView& ent) {
  mol::EntityView non_const_view = ent;
  non_const_view.Apply(*this);
}

void SDFWriter::Write(const mol::EntityHandle& ent) {
  mol::EntityHandle non_const_handle = ent;
  non_const_handle.Apply(*this);
}

bool SDFWriter::VisitChain(const mol::ChainHandle& chain) {
  // print end of molecule line
  if(counter_ != 0) {
    ostr_ << "$$$$" << std::endl;
    counter_ = 0;
    atom_indices_.clear();
  }

  // print header lines
  ostr_ << chain.GetName().substr(6) << std::endl;
  ostr_ << std::endl;
  ostr_ << std::endl;

  // print counts line
  ostr_ << format("%3d") % chain.GetAtomCount()
        << format("%3d") % chain.GetBondCount()
        << "  0  0  0  0            999 V2000"
        << std::endl;

  // write atom block
  SDFAtomWriter atom_writer(ostr_, atom_indices_);
  mol::ChainHandle non_const_chain = chain;
  non_const_chain.Apply(atom_writer);

  // write bond block
  SDFBondWriter bond_writer(ostr_, atom_indices_);
  non_const_chain.Apply(bond_writer);

  // write property block
  //TODO: write property block
  ostr_ << "M  END" << std::endl;

  // write data block
  std::map<String,GenericPropValue> prop_map = non_const_chain.GetPropMap();
  std::map<String,GenericPropValue>::iterator iter;
  for(iter = prop_map.begin(); iter != prop_map.end(); ++iter) {
    ostr_ << "> <" << (*iter).first << ">" << std::endl;
    ostr_ << (*iter).second << std::endl;
    ostr_ << std::endl;
  }

  // write molecule endline
  ostr_ << "$$$$" << std::endl;

  return true;
}

}}
