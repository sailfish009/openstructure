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
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>

#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

#include <ost/profile.hh>
#include <ost/log.hh>
#include <ost/message.hh>

#include <ost/conop/conop.hh>
#include <ost/geom/mat3.hh>
#include <ost/io/io_exception.hh>
#include "pdb_reader.hh"



namespace ost { namespace io {

using boost::format;

namespace {

std::pair<bool, Real> my_strtof(const char* start, int len)
{

  Real n=0;
  bool empty=true;
  int sig=1;
  bool after_dot=false;
  Real factor=0.1;
  // skip empty characters... at beginning
  for (const char* c=start;*c!=0 && ((c-start)<len); ++c) {
    if (empty && (*c)==' ') {
      continue;
    }
    if (*c=='-' && empty) {
      empty=false;
      sig=-1;
      continue;
    }
    if (*c=='.') {
      if (after_dot==true) {
        return std::make_pair(false, 0.0);
      }
      after_dot=true;
      continue;
    }
    if (*c>='0' && *c<='9') {
      empty=false;
      if (after_dot==true) {
        n+=factor*int(*c-'0');
        factor*=0.1;
      } else {
        n=n*10+int(*c-'0');
      }
      continue;
    }
    return std::make_pair(false, 0.0);
  }
  if (empty) {
    return std::make_pair(false, 0.0);
  }
  return std::make_pair(true, sig*n);
}

std::pair<bool, int> my_strtoi(const char* start, int len)
{

  int n=0;
  bool empty=true;
  int sig=1;
  // skip empty characters... at beginning
  for (const char* c=start;*c!=0 && ((c-start)<len); ++c) {
    if (empty && (*c)==' ') {
      continue;
    }
    if (*c=='-' && empty) {
      empty=false;
      sig=-1;
      continue;
    }
    if (*c>='0' && *c<='9') {
      empty=false;
      n=n*10+int(*c-'0');
      continue;
    }
    return std::make_pair(false, 0);
  }
  if (empty) {
    return std::make_pair(false, 0);
  }
  return std::make_pair(true, sig*n);
}

mol::ResNum to_res_num(int num, char ins_code)
{
  return mol::ResNum(num, ins_code==' ' ? '\0' : ins_code);
}

}

PDBReader::PDBReader(std::istream& instream):
  infile_(), instream_(instream), flags_(0)
{
  this->Init(boost::filesystem::path(""));
}

PDBReader::PDBReader(const String& filename)
  : infile_(filename), instream_(infile_), flags_(0)
{
  this->Init(boost::filesystem::path(filename));
}

PDBReader::PDBReader(const boost::filesystem::path& loc):
  infile_(loc), instream_(infile_), flags_(0)
{
  this->Init(loc);
}

void PDBReader::Init(const boost::filesystem::path& loc)
{
  if (boost::iequals(".gz", boost::filesystem::extension(loc))) {
    in_.push(boost::iostreams::gzip_decompressor());
  }
  in_.push(instream_);
  if(!infile_) throw IOException("could not open "+loc.string());
  line_num_=0;
  collect_sequential_atoms_=false;

  if(boost::iequals(boost::filesystem::extension(loc), ".pqr")) {
    is_pqr_=true;
  } else {
    is_pqr_=false;
  }
}


bool PDBReader::HasNext()
{
  // since helix and sheet have to appear before any atom/hetatm records
  // a HELIX/SHEET entry implies a next model.
  while (std::getline(in_, curr_line_) && ++line_num_) {
     if (boost::iequals(curr_line_.substr(0, 4),"ATOM") ||
         (!(flags_ & PDB::NO_HETATMS) &&
          boost::iequals(curr_line_.substr(0, 6),"HETATM")) ||
          boost::iequals(curr_line_.substr(0, 6),"ANISOU") ||
         boost::iequals(curr_line_.substr(0, 5), "SHEET") ||
         boost::iequals(curr_line_.substr(0, 5), "HELIX")) {
       return true;
     } else if (boost::iequals(curr_line_, "END")) {
       return false;
     }
  }
  return false;
}

void PDBReader::Import(mol::EntityHandle& ent,
                       const String& restrict_chains)
{
  Profile profile_import("PDBReader::Import");
  this->ClearState();
  // first read curr_line and then read next...
  restrict_chains_=restrict_chains;
  do {
    if (boost::iequals(curr_line_.substr(0, 4), "ATOM")) {
      LOGN_TRACE("processing ATOM entry");
      ParseAndAddAtom(curr_line_, line_num_, ent, "ATOM");
    } else if(boost::iequals(curr_line_.substr(0, 6), "HETATM")) {
      if (flags_ & PDB::NO_HETATMS)
        continue;
      LOGN_TRACE("processing HETATM entry");
      ParseAndAddAtom(curr_line_, line_num_, ent, "HETATM");
    } else if(boost::iequals(curr_line_.substr(0, 6), "ANISOU")) {
      if (flags_ & PDB::NO_HETATMS)
        continue;
      LOGN_TRACE("processing ANISOU entry");
      ParseAndAddAtom(curr_line_, line_num_, ent, "ANISOU");
    }else if (boost::iequals(curr_line_.substr(0, 6), "ENDMDL")) {
      break;
    } else if (boost::iequals(curr_line_, "END")) {
      break;
    } else if(boost::iequals(curr_line_.substr(0, 5),"HELIX")) {
      ParseHelixEntry(curr_line_);
    } else if(boost::iequals(curr_line_.substr(0, 5),"SHEET")) {
      ParseStrandEntry(curr_line_);
    }
  }  while (std::getline(in_, curr_line_) && ++line_num_);
  LOGN_MESSAGE("imported "
               << chain_count_ << " chains, "
               << residue_count_ << " residues, "
               << atom_count_ << " atoms; with "
               << helix_list_.size() << " helices and "
               << strand_list_.size() << " strands");
  // and now for the fun part, assigning the strands/helices to residues
  for (HSList::const_iterator i=helix_list_.begin(), e=helix_list_.end();
       i!=e; ++i) {
    mol::ChainHandle chain=ent.FindChain(i->chain);
    if (!chain.IsValid()) {
      LOGN_MESSAGE("ignoring helix record for unknown chain "+i->chain);
      continue;
    }
    mol::SecStructure alpha(mol::SecStructure::ALPHA_HELIX);
    // some PDB files contain helix/strand entries that are adjacent to each 
    // other. To avoid visual artifacts, we effectively shorten the first of the 
    // two secondary structure segments to insert one residue of coil 
    // conformation.
    mol::ResNum start=i->start, end=i->end;
    if (helix_list_.end()!=i+1 && (*(i+1)).start.GetNum()<=end.GetNum()+1) {
      end=mol::ResNum((*(i+1)).start.GetNum()-2);
    }
    chain.AssignSecondaryStructure(alpha, start, end);
  }

  for (HSList::const_iterator i=strand_list_.begin(), e=strand_list_.end();
       i!=e; ++i) {
    mol::ChainHandle chain=ent.FindChain(i->chain);
    if (!chain.IsValid()) {
      LOGN_MESSAGE("ignoring strand record for unknown chain "+i->chain);
      continue;
    }
    mol::SecStructure extended(mol::SecStructure::EXTENDED);
    mol::ResNum start=i->start, end=i->end;
    // see comment for helix assignment
    if (strand_list_.end()!=i+1 && (*(i+1)).start.GetNum()<=end.GetNum()+1) {
      end=mol::ResNum((*(i+1)).start.GetNum()-2);
    }    
    chain.AssignSecondaryStructure(extended, start, end);
  }
}

void PDBReader::ClearState()
{
  curr_chain_=mol::ChainHandle();
  curr_residue_=mol::ResidueHandle();
  chain_count_=0;
  residue_count_=0;
  atom_count_=0;
  helix_list_.clear();
  strand_list_.clear();
  sequential_atom_list_.clear();
}

void PDBReader::CollectSequentialAtoms(bool f)
{
  collect_sequential_atoms_=f;
}

std::vector<mol::AtomHandle> PDBReader::GetSequentialAtoms() const
{
  return sequential_atom_list_;
}

void PDBReader::SetFlags(PDBFlags flags)
{
  flags_=flags;
}

void PDBReader::ParseAndAddAtom(const String& line, int line_num,
                                mol::EntityHandle& ent, const String& record_type)
{
  mol::XCSEditor editor=ent.RequestXCSEditor(mol::BUFFERED_EDIT);
  char ins_c, alt_loc;
  char res_key[5], c_name;
  String a_name;
  std::pair<bool, Real> charge, radius;
  std::pair<bool, Real> occ, temp;
  geom::Vec3 apos;
  double anisou[6];
  c_name=line[21];
  if (restrict_chains_.size()>0 &&
      restrict_chains_.find(c_name)==String::npos) {
    return;
  }
  memset(anisou, 0, sizeof(anisou));
  memset(res_key, 0, sizeof(res_key));
  char buf[6];

  std::pair<bool, int> a_num=my_strtoi(line.c_str()+6, 5);
  if (!a_num.first) {
    if (flags_ & PDB::SKIP_FAULTY_RECORDS) {
      return;
    }
    throw IOException(str(format("invalid atom number on line %d") %line_num));
  }
  a_name.assign(line.c_str()+12, 4);
  boost::algorithm::trim(a_name);
  alt_loc=line[16];
  strncpy(res_key, line.c_str()+17, 3);
  res_key[4]='\0';
  buf[4]='\0';
  std::pair<bool, int> res_num=my_strtoi(line.c_str()+22, 4);
  if (!res_num.first) {
    if (flags_ & PDB::SKIP_FAULTY_RECORDS) {
      return;
    }
    throw IOException(str(format("invalid res number on line %d") % line_num));
  }
  ins_c=line[26];
  if (record_type=="ANISOU") {
    for (int i=0;i<6; ++i) {
      std::pair<bool, double> result=my_strtoi(line.c_str()+29+i*7, 6);
      if (!result.first) {
        if (flags_ & PDB::SKIP_FAULTY_RECORDS) {
          return;
        }
        throw IOException(str(format("invalid ANISOU record on line %d")%line_num));
      }
      anisou[i]=result.second;
    }
  }else {
    for (int i=0;i<3; ++i) {
      std::pair<bool, double> result=my_strtof(line.c_str()+30+i*8, 8);
      if (!result.first) {
        if (flags_ & PDB::SKIP_FAULTY_RECORDS) {
          return;
        }
        throw IOException(str(format("invalid coordinate on line %d")%line_num));
      }
      apos[i]=result.second;
    }
  }
  if (record_type=="ANISOU") {
    String aname(a_name);
    if (!curr_residue_.IsValid()) {
      if (flags_ & PDB::SKIP_FAULTY_RECORDS) {
        return;
      } 
      const char* fmt_str="invalid ANISOU record for inexistent atom on line %d";
      throw IOException(str(format(fmt_str) % line_num));      
    }
    mol::AtomHandle atom=curr_residue_.FindAtom(aname);
    if (!atom.IsValid()) {
      if (flags_ & PDB::SKIP_FAULTY_RECORDS) {
        return;
      } 
      const char* fmt_str="invalid ANISOU record for inexistent atom on line %d";
      throw IOException(str(format(fmt_str) % line_num));      
    }
    //get properties which are already set and extend them by adding the ANISOU info
    mol::AtomProp aprop=atom.GetProp();
    geom::Mat3 mat(anisou[0],
                  anisou[3],
                  anisou[4],
                  anisou[3],
                  anisou[1],
                  anisou[5],
                  anisou[4],
                  anisou[5],
                  anisou[2]);
    aprop.anisou=mat;
    //divide by 10**4 to actually reflect the real values
    aprop.anisou/=10000;
    aprop.has_anisou=true;
    atom.SetProp(aprop);
    return;
  }
  // b-factors and occ are replaced by radius and charge if PQR file format
  if(is_pqr_) {
    occ=std::make_pair(true, 1.0);
    temp=std::make_pair(true, 0.0);
    charge=my_strtof(line.c_str()+54,6);
    radius=my_strtof(line.c_str()+60, 6);
  } else {
    occ=my_strtof(line.c_str()+54,6);
    temp=my_strtof(line.c_str()+60, 6);
  }
  LOGN_TRACE( "line: [" << line << "]" );
  String s_ele;
  String s_chain(1, c_name);
  String aname(a_name);
  const char* rk=res_key;
  while(*rk==' ') {
    ++rk;
  }
  mol::ResidueKey rkey(rk);
  // determine element from element column (77-78, right justified) if present
  // otherwise set to empty String. It is up to the builder to determine the
  // correct element in that case.
  if (line.length()>=78) {                          // element column present
    if(line[76]==' ' && line[77]==' ') {            // both characters are empty
      s_ele="";
    } else if(line[76]!=' ' || line[77]!=' ') {     // at least one character not
                                                    // empty
      if(line[76]==' ' && line[77]!=' ') {          // single character element,
                                                    // right justified
        s_ele=line.substr(77,1);
      } else if(line[76]!=' ' && line[77]==' ') {   // single character element,
                                                    // left justified
        s_ele=line.substr(76,1);
      } else {                                      // Real character element
        s_ele=line.substr(76,2);
      }
    }
  }
  else {
    s_ele="";                                       // no element column present
  }

  // some postprocessing
  LOGN_TRACE( "s_chain: [" << c_name << "]" );

  mol::ResNum rnum(to_res_num(res_num.second, ins_c));

  // determine chain and residue update
  bool update_chain=false;
  bool update_residue=false;
  if(!curr_chain_) {
    update_chain=true;
    update_residue=true;
  } else if(curr_chain_.GetName()!=s_chain) {
    update_chain=true;
    update_residue=true;
  }

  if(!curr_residue_) {
    update_residue=true;
  } else if(curr_residue_.GetNumber()!=rnum) {
    update_residue=true;
  }

  if(update_chain) {
    if (!(curr_chain_=ent.FindChain(s_chain))) {
      LOGN_DUMP("new chain " << s_chain);
      curr_chain_=editor.InsertChain(s_chain);
      ++chain_count_;
    }
  }

  if(update_residue) {
    LOGN_DUMP("new residue " << rkey << " " << rnum);
    curr_residue_=editor.AppendResidue(curr_chain_, rkey, rnum);
    assert(curr_residue_.IsValid());
    ++residue_count_;
  }

  // finally add atom
  LOGN_DUMP("adding atom " << aname << " (" << s_ele << ") @" << apos);
  mol::AtomProp aprop;
  aprop.element=s_ele;
  if(is_pqr_) {
    if (radius.first) {
      aprop.radius=radius.second;
    } else {
      aprop.radius=0.0;
    }
  } else {
    aprop.radius=0.0;
  }

  if (temp.first) {
    aprop.b_factor=temp.second;
  }
  if (occ.first) {
    aprop.occupancy=occ.second;
  }
  if (charge.first) {
    aprop.charge=charge.second;
  }

  aprop.is_hetatm=record_type=="HETATM" ? true : false;

  if (alt_loc!=' ') {
    // Check if there is already a atom with the same name.
    mol::AtomHandle me=curr_residue_.FindAtom(aname);
    if (me.IsValid()) {
      try {
        editor.AddAltAtomPos(String(1, alt_loc), me, apos);
      } catch (Error) {
        LOGN_MESSAGE("Ignoring atom alt location since there is already an atom "
                     "with name " << aname << ", but without an alt loc");
        return;
      }
    } else {
      mol::AtomHandle ah=editor.InsertAltAtom(curr_residue_, aname,
                                              String(1, alt_loc), apos, aprop);
      if (collect_sequential_atoms_) {
        sequential_atom_list_.push_back(ah);
      }
      ++atom_count_;
    }
  } else {
    mol::AtomHandle ah = editor.InsertAtom(curr_residue_, aname, apos, aprop);
    if (collect_sequential_atoms_) {
      sequential_atom_list_.push_back(ah);
    }
    ++atom_count_;
  }
}

void PDBReader::ParseHelixEntry(const String& line)
{
  std::pair<bool, int>  start_num=my_strtoi(line.c_str()+21, 4);
  std::pair<bool, int>  end_num=my_strtoi(line.c_str()+33, 4);
  if (!start_num.first || !end_num.first) {
    throw IOException(str(format("invalid helix entry on line") % line_num_));
  }
  LOGN_DEBUG("making helix entry: " << start_num.second << ", " << line[25] << " " << end_num.second << " " << line[37]);
  HSEntry hse = {to_res_num(start_num.second, line[25]),
                 to_res_num(end_num.second, line[37]),
                 line.substr(19,1)};
  if (restrict_chains_.size()==0 ||
      restrict_chains_.find(hse.chain)!=String::npos) {
    helix_list_.push_back(hse);
  }

}

void PDBReader::ParseStrandEntry(const String& line)
{
  std::pair<bool, int>  start_num=my_strtoi(line.c_str()+22, 4);
  std::pair<bool, int>  end_num=my_strtoi(line.c_str()+33, 4);
  if (!start_num.first || !end_num.first) {
    throw IOException(str(format("invalid strand entry on line") % line_num_));
  }
  LOGN_DEBUG("making strand entry: " << start_num.second << ", " << line[26] << " " << end_num.second << " " << line[37]);
  HSEntry hse = {to_res_num(start_num.second, line[26]),
                 to_res_num(end_num.second, line[37]),
                 line.substr(21,1)};
  if (restrict_chains_.size()==0 ||
     restrict_chains_.find(hse.chain)!=String::npos) {
   strand_list_.push_back(hse);
  }
}

}}
