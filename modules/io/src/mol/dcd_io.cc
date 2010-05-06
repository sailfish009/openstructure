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
  Authors: Ansgar Philippsen, Marco Biasini
*/

#include <fstream>
#include <algorithm>

#include <boost/iostreams/filter/gzip.hpp>
#include <boost/filesystem/convenience.hpp>

#include <ost/stdint.hh>
#include <ost/log.hh>
#include <ost/profile.hh>
#include <ost/conop/conop.hh>
#include <ost/mol/xcs_editor.hh>

#include <ost/io/io_exception.hh>
#include <ost/io/swap_util.hh>
#include "load_entity.hh"
#include "dcd_io.hh"
#include "pdb_reader.hh"
#include "pdb_writer.hh"
#include "entity_io_crd_handler.hh"

namespace ost { namespace io {

/* 
   icntrl[1]: number of coordinate sets in file
   icntrl[2]: number of previous dynamic steps
   icntrl[3]: frequency for saving coordinates
   icntlr[4]: number of steps for creation run
*/

namespace {

struct DCDHeader {
  char hdrr[4];
  int icntrl[20];
  int ntitle;
  char title[1024];
  int num, istep, freq,nstep;
  int t_atom_count,f_atom_count, atom_count;
};

bool less_index(const mol::AtomHandle& a1, const mol::AtomHandle& a2)
{
  return a1.GetIndex()<a2.GetIndex();
}

mol::CoordGroupHandle load_dcd(const mol::AtomHandleList& alist2,
			       const String& trj_fn,
			       unsigned int stride)
{
  Profile profile_load("LoadCHARMMTraj");

  mol::AtomHandleList alist(alist2);
  std::sort(alist.begin(),alist.end(),less_index);

  bool gap_flag = true;

  boost::filesystem::path trj_f(trj_fn);
  boost::filesystem::ifstream ff(trj_f);
  
  DCDHeader header;
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

  LOGN_DEBUG("LoadCHARMMTraj: " << header.num << " trajectories with " 
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

  int i=0;
  for(;i<header.num;++i) {
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

  LOGN_VERBOSE("Loaded " << cg.GetFrameCount() << " frames with " << cg.GetAtomCount() << " atoms each");

  return cg;
}

} // anon ns

mol::CoordGroupHandle LoadCHARMMTraj(const String& crd_fn,
                                     const String& trj_fn,
                                     unsigned int stride)
{
  mol::EntityHandle ent=LoadEntity(crd_fn);
  return load_dcd(ent.GetAtomList(),trj_fn,stride);
}


mol::CoordGroupHandle DLLEXPORT_OST_IO LoadCHARMMTraj(const mol::EntityHandle& e,
						      const String& trj_fn,
						      unsigned int stride)
{
  return load_dcd(e.GetAtomList(),trj_fn,stride);
}


namespace {

void write_dcd_hdr(std::ofstream& out,
                   const mol::CoordGroupHandle& coord_group, 
                   unsigned int stepsize)
{
  int32_t magic_number=84;
  char crd[]={'C', 'O', 'R', 'D'};
  out.write(reinterpret_cast<char*>(&magic_number), 4);
  out.write(crd, 4);
  int32_t num_frames=coord_group.GetFrameCount()/stepsize;
  out.write(reinterpret_cast<char*>(&num_frames), 4);
  int32_t zero=0;
  // write zero for istart
  out.write(reinterpret_cast<char*>(&zero), 4);
  int32_t one=1;
  // number of timesteps between dcd saves.
  out.write(reinterpret_cast<char*>(&one), 4);
  // write spacer of 5 blank integers
  for (int i=0; i<5; ++i) {
    out.write(reinterpret_cast<char*>(&zero), 4);
  }
  // write number of fixed atoms

  out.write(reinterpret_cast<char*>(&zero), 4);
  float delta=1.0;
  out.write(reinterpret_cast<char*>(&delta), 4);
  // write spacer of 10 blank integers
  for (int i=0; i <10; ++i) {
    out.write(reinterpret_cast<char*>(&zero), 4);
  }
  out.write(reinterpret_cast<char*>(&magic_number), 4);
  // we don't write any titles for now. This means that the block has only to 
  // accomodate one int, harr, harr, harr.
  int32_t title_block_size=4;
  out.write(reinterpret_cast<char*>(&title_block_size), 4);
  out.write(reinterpret_cast<char*>(&zero), 4);
  int32_t four=4;
  // again block size for titles?
  out.write(reinterpret_cast<char*>(&four), 4);
  // has to be 4
  out.write(reinterpret_cast<char*>(&four), 4);
  int32_t atom_count=coord_group.GetAtomCount();  
  out.write(reinterpret_cast<char*>(&atom_count), 4);
  out.write(reinterpret_cast<char*>(&four), 4);
}

}

void SaveCHARMMTraj(const mol::CoordGroupHandle& coord_group, 
                    const String& pdb_filename, const String& dcd_filename,
                    unsigned int stepsize)
{  
  if(stepsize==0) stepsize=1;
  if(!pdb_filename.empty()) {
    PDBWriter writer(pdb_filename);
    writer.Write(coord_group.GetAtomList());
  }
  std::ofstream out(dcd_filename.c_str(), std::ios::binary);
  write_dcd_hdr(out, coord_group,stepsize);
  int frame_count=coord_group.GetFrameCount();
  int atom_count=coord_group.GetAtomCount();
  
  std::vector<float> x(atom_count);
  std::vector<float> y(atom_count);
  std::vector<float> z(atom_count);
  int32_t out_n=atom_count*4;
  for (int i=0; i<frame_count; i+=stepsize) {
    mol::CoordFramePtr frame=coord_group.GetFrame(i);
    int k=0;
    for (mol::CoordFrame::iterator j=frame->begin(), 
         e=frame->end(); j!=e; ++j, ++k) {
      //geom::Vec3 v=*j;
      x[k]=float((*j)[0]);
      y[k]=float((*j)[1]);
      z[k]=float((*j)[2]);
    }
    out.write(reinterpret_cast<char*>(&out_n), 4);
    out.write(reinterpret_cast<char*>(&x[0]), out_n);
    out.write(reinterpret_cast<char*>(&out_n), 4);
    out.write(reinterpret_cast<char*>(&out_n), 4);
    out.write(reinterpret_cast<char*>(&y[0]), out_n);
    out.write(reinterpret_cast<char*>(&out_n), 4);
    out.write(reinterpret_cast<char*>(&out_n), 4);
    out.write(reinterpret_cast<char*>(&z[0]), out_n);
    out.write(reinterpret_cast<char*>(&out_n), 4);
  }
}

}} // ns
