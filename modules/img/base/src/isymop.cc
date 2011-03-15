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
#include "isymop.hh"

namespace ost { namespace img {


ISymop::ISymop(const Symop& symop, const Size& size)
{
  for (size_t i=0; i<3; ++i) {
    for (size_t j=0; j<3; ++j) {
      rot_(i, j)=static_cast<int>(rint(symop.Rot()(i, j)));
    }
  }
  trans_[0]=static_cast<int>(rint(symop.Trans()[0]*size[0]));
  trans_[1]=static_cast<int>(rint(symop.Trans()[1]*size[1]));
  trans_[2]=static_cast<int>(rint(symop.Trans()[2]*size[2]));
}

}}
