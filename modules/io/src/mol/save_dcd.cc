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

#include <fstream>

#include <ost/io/mol/pdb_writer.hh>
#include <ost/io/mol/save_dcd.hh>
#include <ost/stdint.hh>
/*
  Author: Marco Biasini
 */
namespace ost { namespace io {
  

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
  PDBWriter writer(pdb_filename);
  writer.Write(coord_group.GetAtomList());
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

}}
