//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2011 by the OpenStructure authors
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

bool read_dcd_header(std::istream& istream, DCDHeader& header, bool& swap_flag, 
                     bool& skip_flag, bool& gap_flag)
{
  if (!istream) {
    return false;
  }
  char dummy[4];  
  gap_flag=true;
  swap_flag=false;
  skip_flag=false;
  if(gap_flag) istream.read(dummy,sizeof(dummy));
  istream.read(header.hdrr,sizeof(char)*4);
  istream.read(reinterpret_cast<char*>(header.icntrl),sizeof(int)*20);
  if(header.icntrl[1]<0 || header.icntrl[1]>1e8) {
    // nonsense atom count, try swapping
    swap_int(header.icntrl,20);
    if(header.icntrl[1]<0 || header.icntrl[1]>1e8) {
      throw(IOException("LoadCHARMMTraj: nonsense atom count in header"));
    } else {
      LOG_VERBOSE("LoadCHARMMTraj: byte-swapping");
      swap_flag=true;
    }
  }

  if(header.icntrl[19]!=0) { // CHARMM format
    skip_flag=(header.icntrl[10]!=0);
    if(skip_flag) {
      LOG_VERBOSE("LoadCHARMMTraj: using CHARMM format with per-frame header");
    } else {
      LOG_VERBOSE("LoadCHARMMTraj: using CHARMM format");
    }
  } else {
    // XPLOR format
    LOG_VERBOSE("LoadCHARMMTraj: using XPLOR format");
  }
  if(gap_flag) istream.read(dummy,sizeof(dummy));
  if (!istream) {
    return false;
  }
  istream.read(reinterpret_cast<char*>(&header.ntitle),sizeof(int));
  if (!istream) {
    return false;
  }
  if(swap_flag) swap_int(&header.ntitle,1);
  if(gap_flag) istream.read(dummy,sizeof(dummy));

  istream.read(header.title,sizeof(char)*header.ntitle);
  header.title[header.ntitle]='\0';
  if(gap_flag) istream.read(dummy,sizeof(dummy));
  istream.read(reinterpret_cast<char*>(&header.t_atom_count),sizeof(int));
  if(swap_flag) swap_int(&header.t_atom_count,1);
  if(gap_flag) istream.read(dummy,sizeof(dummy));
  header.num=header.icntrl[0];
  header.istep=header.icntrl[1];
  header.freq=header.icntrl[2];
  header.nstep=header.icntrl[3];
  header.f_atom_count=header.icntrl[8];
  header.atom_count=header.t_atom_count-header.f_atom_count;
  return true;
}


size_t calc_frame_size(bool skip_flag, bool gap_flag, size_t num_atoms)
{
  size_t frame_size=0;
  if (skip_flag) {
    frame_size+=14*sizeof(int);
  }
  if (gap_flag) {
    frame_size+=6*sizeof(int);
  }
  frame_size+=3*sizeof(float)*num_atoms;
  return frame_size;
}

  
bool read_frame(std::istream& istream, const DCDHeader& header, 
                size_t frame_size, bool skip_flag, bool gap_flag, 
                bool swap_flag, std::vector<float>& xlist,
                std::vector<geom::Vec3>& frame,
                uint frame_num,geom::Vec3& cell_size,geom::Vec3& cell_angles)
{
  char dummy[4];
  //if(skip_flag) istream.seekg(14*4,std::ios_base::cur);
  if(skip_flag){
    istream.read(dummy,sizeof(dummy));
    istream.read(reinterpret_cast<char*>(&cell_size[0]),sizeof(float)*3);
    istream.read(reinterpret_cast<char*>(&cell_angles[0]),sizeof(float)*3);
    istream.read(dummy,sizeof(dummy));
  }

  // read each frame
  if(!istream) {
    /* premature EOF */
    LOG_ERROR("LoadCHARMMTraj: premature end of file while trying to read frame "
              << frame_num << "Nothing left to be read");
    return false;
  }
  // x coord
  if(gap_flag) istream.read(dummy,sizeof(dummy));
  istream.read(reinterpret_cast<char*>(&xlist[0]),sizeof(float)*xlist.size());
  if(swap_flag) swap_float(&xlist[0],xlist.size());
  if(gap_flag) istream.read(dummy,sizeof(dummy));
  for(uint j=0;j<frame.size();++j) {
    frame[j].x=xlist[j];
  }
  if(!istream) {
    /* premature EOF */
    LOG_ERROR("LoadCHARMMTraj: premature end of file while trying to read frame "
              << frame_num << ". No y coordinates");
    return false;
  }
  // y coord
  if(gap_flag) istream.read(dummy,sizeof(dummy));
  istream.read(reinterpret_cast<char*>(&xlist[0]),sizeof(float)*xlist.size());
  if(swap_flag) swap_float(&xlist[0],xlist.size());
  if(gap_flag) istream.read(dummy,sizeof(dummy));
  for(uint j=0;j<frame.size();++j) {
    frame[j].y=xlist[j];
  }
  if(!istream) {
    /* premature EOF */
    LOG_ERROR("LoadCHARMMTraj: premature end of file while trying to read frame "
              << frame_num << ". No z coordinates");
    return false;
  }
  // z coord
  if(gap_flag) istream.read(dummy,sizeof(dummy));
  istream.read(reinterpret_cast<char*>(&xlist[0]),sizeof(float)*xlist.size());
  if(swap_flag) swap_float(&xlist[0],xlist.size());
  if(gap_flag) istream.read(dummy,sizeof(dummy));
  for(uint j=0;j<frame.size();++j) {
    frame[j].z=xlist[j];
  }
  if(!istream) {
    /* premature EOF */
    LOG_ERROR("LoadCHARMMTraj: premature end of file while trying to read frame "
              << frame_num);
    return false;
  }
  return true;
}

bool read_frame(std::istream& istream, const DCDHeader& header, 
                size_t frame_size, bool skip_flag, bool gap_flag, 
                bool swap_flag, std::vector<float>& xlist,
                std::vector<geom::Vec3>& frame,uint frame_num)
{
  geom::Vec3 cell_size=geom::Vec3(),cell_angles=geom::Vec3();
  return read_frame(istream,header, frame_size,skip_flag, gap_flag, 
                    swap_flag, xlist,frame,frame_num, cell_size, cell_angles);
}
  
  
mol::CoordGroupHandle load_dcd(const mol::AtomHandleList& alist, // this atom list is already sorted!
                               const String& trj_fn,
                               unsigned int stride)
{
  std::ifstream istream(trj_fn.c_str(), std::ios::binary);
  if(!istream) {
    std::ostringstream msg;
    msg << "LoadCHARMMTraj: cannot open " << trj_fn;
    throw(IOException(msg.str()));
  }
  Profile profile_load("LoadCHARMMTraj");

  DCDHeader header; 
  bool swap_flag=false, skip_flag=false, gap_flag=false;
  read_dcd_header(istream, header, swap_flag, skip_flag, gap_flag);
  if(alist.size() != static_cast<size_t>(header.t_atom_count)) {
    LOG_ERROR("LoadCHARMMTraj: atom count missmatch: " << alist.size() 
               << " in coordinate file, " << header.t_atom_count 
               << " in each traj frame");
    throw(IOException("invalid trajectory"));
  }

  mol::CoordGroupHandle cg=CreateCoordGroup(alist);
  std::vector<geom::Vec3> clist(header.t_atom_count);
  std::vector<float> xlist(header.t_atom_count);
  geom::Vec3 cell_size, cell_angles;
  size_t frame_size=calc_frame_size(skip_flag, gap_flag, xlist.size());
  int i=0;
  for(;i<header.num;i+=stride) {
    std::cout << i << " " << header.num << std::endl;
    if (!read_frame(istream, header, frame_size, skip_flag, gap_flag, 
                    swap_flag, xlist, clist, i,cell_size,cell_angles)) {
      break;
    }
    if(skip_flag) {
      cg.AddFrame(clist,cell_size,cell_angles);
    }
    else cg.AddFrame(clist);

    // skip frames (defined by stride)
    if(stride>1) istream.seekg(frame_size*(stride-1),std::ios_base::cur);
  }
  istream.get();
  if(!istream.eof()) {
    LOG_VERBOSE("LoadCHARMMTraj: unexpected trailing file data, bytes read: " 
                 << istream.tellg());
  }

  LOG_VERBOSE("Loaded " << cg.GetFrameCount() << " frames with "
              << cg.GetAtomCount() << " atoms each");

  return cg;
}

class  DCDCoordSource : public mol::CoordSource {
public:
  DCDCoordSource(const mol::AtomHandleList& atoms, const String& filename, 
                 uint stride): 
    mol::CoordSource(atoms), filename_(filename), 
    stream_(filename.c_str(), std::ios::binary), loaded_(false), stride_(stride)
  {
    frame_count_=0;
    this->SetMutable(false);
    frame_=mol::CoordFramePtr(new mol::CoordFrame(atoms.size()));
  }
    
  
  virtual uint GetFrameCount() 
  { 
    if (!frame_count_)
      const_cast<DCDCoordSource*>(this)->FetchFrame(0);
    return frame_count_; 
  }
  
  virtual mol::CoordFramePtr GetFrame(uint frame_id) const {
    const_cast<DCDCoordSource*>(this)->FetchFrame(frame_id);
    return frame_;
  }

  virtual void AddFrame(const std::vector<geom::Vec3>& coords) {}
  virtual void AddFrame(const std::vector<geom::Vec3>& coords,const geom::Vec3& box_size,const geom::Vec3& box_angles) {}
  virtual void InsertFrame(int pos, const std::vector<geom::Vec3>& coords) {}
private:
  
  void FetchFrame(uint frame);
  String               filename_;
  DCDHeader            header_;
  bool                 skip_flag_;
  bool                 swap_flag_;
  bool                 gap_flag_;
  std::ifstream        stream_;
  bool                 loaded_;
  uint                 frame_count_;
  uint                 curr_frame_;
  uint                 stride_;
  size_t               frame_start_;
  mol::CoordFramePtr   frame_;
};


void DCDCoordSource::FetchFrame(uint frame)
{
  if (!loaded_) {
    read_dcd_header(stream_, header_, swap_flag_, skip_flag_, gap_flag_);
    frame_start_=stream_.tellg();
    loaded_=true;
    frame_count_=header_.num/stride_;
  }
  size_t frame_size=calc_frame_size(skip_flag_, gap_flag_, 
                                    header_.t_atom_count);  
  size_t pos=frame_start_+frame_size*frame*stride_;
  stream_.seekg(pos,std::ios_base::beg);
  std::vector<float> xlist(header_.t_atom_count);
  if (!read_frame(stream_, header_, frame_size, skip_flag_, gap_flag_, 
                  swap_flag_, xlist, *frame_.get(), frame)) {
  }  
}

typedef boost::shared_ptr<DCDCoordSource> DCDCoordSourcePtr;


} // anon ns


mol::CoordGroupHandle LoadCHARMMTraj(const mol::EntityHandle& ent,
                                     const String& trj_fn,
                                     unsigned int stride, bool lazy_load)
{
  mol::AtomHandleList alist(ent.GetAtomList());
  std::sort(alist.begin(),alist.end(),less_index);
  if (lazy_load) {
    LOG_INFO("Importing CHARMM trajectory with lazy_load=true");
    DCDCoordSource* source=new DCDCoordSource(alist, trj_fn, stride);
    return mol::CoordGroupHandle(DCDCoordSourcePtr(source));
  }
  LOG_INFO("Importing CHARMM trajectory with lazy_load=false");  
  return load_dcd(alist, trj_fn, stride);
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
                    unsigned int stepsize, const IOProfile& profile)
{  
  if(stepsize==0) stepsize=1;
  if(!pdb_filename.empty()) {
    PDBWriter writer(pdb_filename, profile);
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
