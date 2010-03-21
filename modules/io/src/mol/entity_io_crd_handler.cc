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
  Author: Ansgar Philippsen
 */
#include <iostream>
#include <sstream>
#include <iomanip>

#include <boost/iostreams/filter/gzip.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>

#include <ost/log.hh>
#include <ost/conop/conop.hh>
#include <ost/mol/xcs_editor.hh>
#include <ost/profile.hh>

#include <ost/io/io_exception.hh>
#include "entity_io_crd_handler.hh"
#include "pdb_reader.hh"
#include <ost/io/swap_util.hh>

namespace ost { namespace io {

using boost::format;

String FORMAT_NAME_STRING;
String FORMAT_DESCRIPTION_STRING;

CRDReader::CRDReader(const boost::filesystem::path& loc):
  sequential_atom_list_(),
  curr_chain_(),
  curr_residue_(),
  chain_count_(0),
  residue_count_(0),
  atom_count_(0),
  infile_(loc),
  in_()
{
  if (boost::iequals(".gz", boost::filesystem::extension(loc))) {
    in_.push(boost::iostreams::gzip_decompressor());    
  }
  in_.push(infile_);  
  if(!infile_) throw IOException("could not open "+loc.string());
}

std::vector<mol::AtomHandle> CRDReader::GetSequentialAtoms() const
{
  return sequential_atom_list_;
}

void CRDReader::Import(mol::EntityHandle& ent)
{
  Profile profile_import("CRDReader::Import");

  String line;
  // skip over comments at the start
  while(std::getline(in_,line)) {
    if(line[0]!='*') break;
  }
  // line contains atom count
  boost::trim(line);
  //int acount = boost::lexical_cast<int>(line);

  while(std::getline(in_,line)) {
    ParseAndAddAtom(line,ent);
  }
  LOGN_MESSAGE("imported " << chain_count_ << " chains, " << residue_count_
                << " residues, " << atom_count_ << " atoms");  
}

void CRDReader::ParseAndAddAtom(const String& line, mol::EntityHandle& ent)
{
  mol::XCSEditor editor=ent.RequestXCSEditor(mol::BUFFERED_EDIT);

  LOGN_TRACE( "line: [" << line << "]" );

  //int anum = boost::lexical_cast<int>(boost::trim_copy(line.substr(0,5)));
  String aname = boost::trim_copy(line.substr(16,4));
  String ele = aname.substr(0,1);
  String rname = boost::trim_copy(line.substr(11,3));
  int irnum = boost::lexical_cast<int>(boost::trim_copy(line.substr(55,4)));
  String s_chain = boost::trim_copy(line.substr(51,4));
  geom::Vec3 apos(boost::lexical_cast<Real>(boost::trim_copy(line.substr(21,9))),
                  boost::lexical_cast<Real>(boost::trim_copy(line.substr(31,9))),
                  boost::lexical_cast<Real>(boost::trim_copy(line.substr(41,9))));

  mol::ResidueKey rkey(rname);
  
  // some postprocessing
  LOGN_TRACE( "s_chain: [" << s_chain << "]" );

  mol::ResNum rnum(irnum);
  
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
  LOGN_DUMP("adding atom " << aname << " (" << ele << ") @" << apos);
  mol::AtomProp aprop;
  aprop.element=ele;
  aprop.radius=conop::Conopology::Instance().GetDefaultAtomRadius(ele);
  aprop.is_hetatm=false;
  aprop.b_factor=0.0;
  aprop.occupancy=1.0;
  
  mol::AtomHandle ah = editor.InsertAtom(curr_residue_, aname, apos, aprop);
  sequential_atom_list_.push_back(ah);
  ++atom_count_;
}



bool EntityIOCRDHandler::RequiresBuilder() const
{
  return true;
}

void EntityIOCRDHandler::Import(mol::EntityHandle& ent,
                                const boost::filesystem::path& loc)
{
  CRDReader reader(loc);
  reader.Import(ent);
}

void EntityIOCRDHandler::Export(const mol::EntityView& ent,
                                const boost::filesystem::path& loc) const
{
}

namespace {

bool crd_handler_is_responsible_for(const boost::filesystem::path& loc,
                                    const String& type) {
  if(type=="auto") {
    String match_suf_string=loc.string();
	std::transform(match_suf_string.begin(),match_suf_string.end(),match_suf_string.begin(),tolower);
    if( detail::FilenameEndsWith(match_suf_string,".crd") || detail::FilenameEndsWith(match_suf_string,".crd.gz") ) {
      return true;
    }
  } else if(type=="crd") {
    return true;
  }

  return false;
}

}

bool EntityIOCRDHandler::ProvidesImport(const boost::filesystem::path& loc,
                                        const String& type)
{
  return crd_handler_is_responsible_for(loc, type);
}

bool EntityIOCRDHandler::ProvidesExport(const boost::filesystem::path& loc,
                                        const String& type)
{
  return false;
}

mol::EntityHandle LoadCRD(const String& file_name) 
{
  Profile profile_load("LoadCRD");
  conop::BuilderP builder = conop::Conopology::Instance().GetBuilder();  
  CRDReader reader(file_name);
  mol::EntityHandle ent=mol::CreateEntity();
  mol::XCSEditor editor=ent.RequestXCSEditor(mol::BUFFERED_EDIT);
  reader.Import(ent);
  conop::Conopology::Instance().ConnectAll(builder,ent);    
  return ent;
}

namespace {

struct TrjHeader {
  char hdrr[4];
  int icntrl[20];
  int ntitle;
  char title[1024];
  int num, istep, freq,nstep;
  int t_atom_count,f_atom_count, atom_count;
};

}

/* 
   icntrl[1]: number of coordinate sets in file
   icntrl[2]: number of previous dynamic steps
   icntrl[3]: frequency for saving coordinates
   icntlr[4]: number of steps for creation run
*/

mol::CoordGroupHandle LoadCHARMMTraj(const String& crd_fn,
                                     const String& trj_fn,
                                     int flags)
{
  Profile profile_load("LoadCHARMMTraj");

  bool gap_flag = true;

  boost::filesystem::path crd_f(crd_fn);
  boost::filesystem::path trj_f(trj_fn);

  

  // first the coordinate reference file
  conop::BuilderP builder = conop::Conopology::Instance().GetBuilder();  
  mol::EntityHandle ent=mol::CreateEntity();
  mol::XCSEditor editor=ent.RequestXCSEditor(mol::BUFFERED_EDIT);
  std::vector<mol::AtomHandle> alist;
  if(boost::filesystem::extension(crd_f)==".pdb") {
    PDBReader reader(crd_f);
    reader.CollectSequentialAtoms(true);
    LOGN_MESSAGE("importing coordinate data");
    reader.Import(ent);
    alist = reader.GetSequentialAtoms();
  } else if (boost::filesystem::extension(crd_f)==".crd") {
    CRDReader reader(crd_f);
    LOGN_MESSAGE("importing coordinate data");
    reader.Import(ent);
    alist = reader.GetSequentialAtoms();
  } else {
    throw(IOException("unsupported coordinate file format"));
  }
  conop::Conopology::Instance().ConnectAll(builder,ent);

  // now the trajectory
  boost::filesystem::ifstream ff(trj_f);
  
  TrjHeader header;
  char dummy[4];
  bool swap_flag=false;

  LOGN_MESSAGE("importing trajectory data");

  if(gap_flag) ff.read(dummy,sizeof(dummy));
  ff.read(header.hdrr,sizeof(char)*4);
  ff.read(reinterpret_cast<char*>(header.icntrl),sizeof(int)*20);

  if(header.icntrl[1]<0 || header.icntrl[1]>1e6) {
    // nonsense atom count, try swapping
    swap_int(header.icntrl,20);
    if(header.icntrl[1]<0 || header.icntrl[1]>1e6) {
      throw(IOException("LoadCHARMMTraj: nonsense atom count in header"));
    } else {
      LOGN_MESSAGE("LoadCHARMMTraj: byte-swapping");
      swap_flag=true;
    }
  }

  bool skip_flag=false;

  if(header.icntrl[19]!=0) { // CHARMM format
    skip_flag=(header.icntrl[10]!=0);
    if(skip_flag) {
      LOGN_VERBOSE("LoadCHARMMTraj: using CHARMM format with per-frame header");
    } else {
      LOGN_VERBOSE("LoadCHARMMTraj: using CHARMM format");
    }
  } else {
    // XPLOR format
    LOGN_VERBOSE("LoadCHARMMTraj: using XPLOR format");
  }

  if(gap_flag) ff.read(dummy,sizeof(dummy));
  ff.read(reinterpret_cast<char*>(&header.ntitle),sizeof(int));
  if(swap_flag) swap_int(&header.ntitle,1);
  if(gap_flag) ff.read(dummy,sizeof(dummy));
  ff.read(header.title,sizeof(char)*header.ntitle);
  header.title[header.ntitle]='\0';
  if(gap_flag) ff.read(dummy,sizeof(dummy));
  ff.read(reinterpret_cast<char*>(&header.t_atom_count),sizeof(int));
  if(swap_flag) swap_int(&header.t_atom_count,1);
  if(gap_flag) ff.read(dummy,sizeof(dummy));
  header.num=header.icntrl[0];
  header.istep=header.icntrl[1];
  header.freq=header.icntrl[2];
  header.nstep=header.icntrl[3];
  header.f_atom_count=header.icntrl[8];
  header.atom_count=header.t_atom_count-header.f_atom_count;

  LOGN_MESSAGE("LoadCHARMMTraj: " << header.num << " trajectories with " 
               << header.atom_count << " atoms (" << header.f_atom_count 
               << " fixed) each");

  if(alist.size() != static_cast<size_t>(header.t_atom_count)) {
    LOGN_ERROR("LoadCHARMMTraj: atom count missmatch: " << alist.size() 
               << " in coordinate file, " << header.t_atom_count 
               << " in each traj frame");
    throw(IOException("invalid trajectory"));
  }

  mol::CoordGroupHandle cg=CreateCoordGroup(alist);
  std::vector<geom::Vec3> clist(header.t_atom_count);
  std::vector<float> xlist(header.t_atom_count);

  for(int i=0;i<header.num;++i) {
    if(skip_flag) ff.seekg(14*4,std::ios_base::cur);
    // read each frame
    if(!ff) {
      /* premature EOF */
      LOGN_ERROR("LoadCHARMMTraj: premature end of file, " << i 
                 << " frames read");
      break;
    }
    // x coord
    if(gap_flag) ff.read(dummy,sizeof(dummy));
    ff.read(reinterpret_cast<char*>(&xlist[0]),sizeof(float)*xlist.size());
    if(swap_flag) swap_float(&xlist[0],xlist.size());
    if(gap_flag) ff.read(dummy,sizeof(dummy));
    for(uint j=0;j<clist.size();++j) {
      clist[j].SetX(xlist[j]);
    }
    // y coord
    if(gap_flag) ff.read(dummy,sizeof(dummy));
    ff.read(reinterpret_cast<char*>(&xlist[0]),sizeof(float)*xlist.size());
    if(swap_flag) swap_float(&xlist[0],xlist.size());
    if(gap_flag) ff.read(dummy,sizeof(dummy));
    for(uint j=0;j<clist.size();++j) {
      clist[j].SetY(xlist[j]);
    }
    // z coord
    if(gap_flag) ff.read(dummy,sizeof(dummy));
    ff.read(reinterpret_cast<char*>(&xlist[0]),sizeof(float)*xlist.size());
    if(swap_flag) swap_float(&xlist[0],xlist.size());
    if(gap_flag) ff.read(dummy,sizeof(dummy));
    for(uint j=0;j<clist.size();++j) {
      clist[j].SetZ(xlist[j]);
    }
    cg.AddFrame(clist);
  }

  ff.get();
  if(!ff.eof()) {
    LOGN_VERBOSE("LoadCHARMMTraj: unexpected trailing file data, bytes read: " 
                 << ff.tellg());
  }
  return cg;
}

void EntityIOCRDHandler::Import(mol::EntityHandle& ent, 
                                std::istream& stream)
{
  throw IOException("CRD format does not support import from stream");
}


void EntityIOCRDHandler::Export(const mol::EntityView& ent, 
                                std::ostream& stream) const
{
  throw IOException("CRD format does not support export to stream");
}

}} // ns


