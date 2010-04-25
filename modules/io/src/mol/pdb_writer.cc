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
  Author: Marco Biasini
 */
#include <boost/format.hpp>
#include <string.h>
#include <ost/io/io_exception.hh>
#include "pdb_writer.hh"

using boost::format;

namespace ost { namespace io {


namespace {

void write_atom(std::ostream& ostr, const mol::AtomHandle& atom, int atomnum, 
                bool is_pqr)
{
  mol::ResidueHandle res=atom.GetResidue();
  char ins_code=res.GetNumber().GetInsCode();
  String record_name=atom.GetAtomProps().is_hetatm ? "HETATM" : "ATOM  ";
  String aname_str=atom.GetName();
  if(aname_str.length()<4) {
    aname_str=" "+aname_str;
  }
  std::vector<String> names=atom.GetAltGroupNames();
  if (names.empty()) {
    geom::Vec3 p=atom.GetPos();
    String ins_str=(ins_code==0 ? " " : String(1, ins_code));
    ostr << record_name << format("%5d") % atomnum
         << format(" %-4s") % aname_str
         << " "
         << format("%.3s") % res.GetKey()
         << format(" %.1s") % res.GetChain().GetName()
         << format("%4d%s") % res.GetNumber().GetNum() %  ins_str
         << "   "
         << format("%8.3f%8.3f%8.3f") % p[0] % p[1] % p[2];
    if(is_pqr) {
      ostr << format("%6.2f") % atom.GetAtomProps().charge
           << format("%6.2f") % atom.GetAtomProps().radius;
    } else {
      ostr << format("%6.2f") % atom.GetAtomProps().occupancy
           << format("%6.2f") % atom.GetAtomProps().b_factor;
    }
    ostr << format("%10s%2s") % "" % atom.GetAtomProps().element
         << std::endl
    ;    
  } else {
    for (std::vector<String>::const_iterator i=names.begin(), 
         e=names.end(); i!=e; ++i) {
       geom::Vec3 p=atom.GetAltPos(*i);
       String ins_str=(ins_code==0 ? " " : String(1, ins_code));
       ostr << record_name << format("%5d") % atomnum
            << format(" %-4s") % aname_str
            << *i
            << format("%.3s") % res.GetKey()
            << format(" %.1s") % res.GetChain().GetName()
            << format("%4d%s") % res.GetNumber().GetNum() %  ins_str
            << "   "
            << format("%8.3f%8.3f%8.3f") % p[0] % p[1] % p[2];
       if(is_pqr) {
         ostr << format("%6.2f") % atom.GetAtomProps().charge
              << format("%6.2f") % atom.GetAtomProps().radius;
       } else {
         ostr << format("%6.2f") % atom.GetAtomProps().occupancy
              << format("%6.2f") % atom.GetAtomProps().b_factor;
       }
       ostr << format("%10s%2s") % "" % atom.GetAtomProps().element
            << std::endl
       ;       
    }
  }
}

class PDBWriterImpl : public mol::EntityVisitor {
public:
  PDBWriterImpl(std::ostream& ostream)
    : ostr_(ostream), counter_(0), is_pqr_(false), last_chain_() {
  }
private:
public:
  virtual bool VisitAtom(const mol::AtomHandle& atom) {
    counter_++;
    if (last_chain_!=atom.GetResidue().GetChain()) {
      if (last_chain_.IsValid()) {
        ostr_ << "TER" << std::endl;
      }
      last_chain_=atom.GetResidue().GetChain();
    }    
    write_atom(ostr_, atom, counter_, is_pqr_);
    return true;
  }
  void SetIsPQR(bool t) {
    is_pqr_=t;
  }
private:
  std::ostream&    ostr_;
  int              counter_;
  bool             is_pqr_;
  mol::ChainHandle last_chain_;
};


}

PDBWriter::PDBWriter(std::ostream& stream):
  outfile_(), outstream_(stream)
{}

PDBWriter::PDBWriter(const boost::filesystem::path& filename):
  outfile_(filename.file_string().c_str()), outstream_(outfile_), mol_count_(0)
{}

PDBWriter::PDBWriter(const String& filename):
  outfile_(filename.c_str()), outstream_(outfile_),  mol_count_(0)
{}

void PDBWriter::WriteModelLeader()
{
  ++mol_count_;
  if (PDB::Flags() & PDB::WRITE_MULTIPLE_MODELS) {
    outstream_ << "MODEL     " << mol_count_ << std::endl;
  } else if (mol_count_>1) {
    throw IOException("Please enable the PDB::WRITE_MULTIPLE_MODELS flag to "
                      "write multiple models");
  }
}

void PDBWriter::WriteModelTrailer()
{
  if (PDB::Flags() & PDB::WRITE_MULTIPLE_MODELS) {
    outstream_ << "ENDMDL" << std::endl;
  }
}

template <typename H>
void PDBWriter::WriteModel(H ent)
{
  this->WriteModelLeader();
  PDBWriterImpl writer(outstream_);
  if (PDB::Flags() & PDB::PQR_FORMAT) {
    writer.SetIsPQR(true);
  }
  ent.Apply(writer);
  this->WriteModelTrailer();
}

void PDBWriter::Write(const mol::EntityView& ent)
{
  this->WriteModel(ent);
}

void PDBWriter::Write(const mol::EntityHandle& ent)
{
  this->WriteModel(ent);
}

void PDBWriter::Write(const mol::AtomHandleList& atoms)
{
  this->WriteModelLeader();
  int counter=1;
  mol::ChainHandle last_chain;
  for (mol::AtomHandleList::const_iterator i=atoms.begin(),
       e=atoms.end(); i!=e; ++i, ++counter) {

    if (last_chain!=(*i).GetResidue().GetChain()) {
      if (last_chain.IsValid()) {
        outstream_ << "TER" << std::endl;
      }
      last_chain=(*i).GetResidue().GetChain();
    }
    write_atom(outstream_, *i, counter, PDB::Flags() & PDB::PQR_FORMAT);      
  }
  this->WriteModelTrailer();
}

  
PDBWriter::~PDBWriter()
{
  outstream_ << "END   ";
}

}}
