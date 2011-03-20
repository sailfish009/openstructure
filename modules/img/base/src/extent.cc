//------------------------------------------------------------------------------
// This file is part of the OpenStructure project <www.openstructure.org>
//
// Copyright (C) 2008-2011 by the OpenStructure authors
// Copyright (C) 2003-2010 by the IPLT authors
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
  Extent implementation

  Author: Ansgar Philippsen
*/

#include <cstdlib>
#include <iostream>

#include <ost/img/module_config.hh>
#include "extent.hh"
#include "vecmat.hh"

namespace ost { namespace img {

// 'global' functions
Extent Overlap(const Extent& e1, const Extent& e2)
{
  Point a,b;

  for(int k=0;k<3;++k) {
    a[k] = std::max(e1.GetStart()[k],e2.GetStart()[k]);
    b[k] = std::min(e1.GetEnd()[k],e2.GetEnd()[k]);
    if(b[k]<a[k]) throw InvalidExtentException("non-overlapping extents in call to Overlap");
  }
  return Extent(a,b);
}

bool HasOverlap(const Extent& e1, const Extent& e2)
{
  Point a,b;

  for(int k=0;k<3;++k) {
    a[k] = std::max(e1.GetStart()[k],e2.GetStart()[k]);
    b[k] = std::min(e1.GetEnd()[k],e2.GetEnd()[k]);
    if(b[k]<a[k]) return false;
  }
  return true;
}



std::ostream& operator<<(std::ostream& os, const Extent &b) 
{
  os << "[" << b.GetStart() << "," << b.GetEnd() << "]";
  return os;
}


}}// namespace
